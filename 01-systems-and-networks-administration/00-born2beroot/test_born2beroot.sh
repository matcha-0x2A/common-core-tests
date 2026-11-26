#!/usr/bin/env bash

# ==============================================================================
# 42 Common Core — Born2beRoot Full Compliance & Audit Test Suite
# ==============================================================================

set -uo pipefail

# ANSI Colors
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
PURPLE="\033[0;35m"
CYAN="\033[0;36m"
BOLD="\033[1m"
DIM="\033[2m"
RESET="\033[0m"

PASS="${GREEN}✔ PASS${RESET}"
FAIL="${RED}✖ FAIL${RESET}"
WARN="${YELLOW}⚠ WARN${RESET}"
INFO="${BLUE}ℹ INFO${RESET}"

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
WARNING_TESTS=0

record_result() {
    local status="$1"
    local name="$2"
    local details="${3:-}"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    if [ "$status" = "PASS" ]; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo -e "  [ ${PASS} ] ${BOLD}${name}${RESET}"
    elif [ "$status" = "WARN" ]; then
        WARNING_TESTS=$((WARNING_TESTS + 1))
        echo -e "  [ ${WARN} ] ${BOLD}${name}${RESET}"
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo -e "  [ ${FAIL} ] ${BOLD}${name}${RESET}"
    fi

    if [ -n "$details" ]; then
        echo -e "         ${DIM}${details}${RESET}"
    fi
}

banner() {
    echo -e "${CYAN}${BOLD}"
    echo "=================================================================="
    echo "       42 COMMON CORE — BORN2BEROOT AUTOMATED TEST SUITE          "
    echo "=================================================================="
    echo -e "${RESET}"
}

section() {
    echo -e "\n${BOLD}${PURPLE}▶ $1${RESET}"
    echo -e "${DIM}------------------------------------------------------------------${RESET}"
}

# ------------------------------------------------------------------------------
# 1. Environment & Credentials Detection
# ------------------------------------------------------------------------------
banner

NEED_EXPORT_HINT=0

# Username
if [ -z "${B2BR_USER:-}" ]; then
    DEFAULT_USER="mchambon"
    if [ -n "${USER:-}" ] && [ "$USER" != "root" ]; then
        DEFAULT_USER="$USER"
    fi
    read -p "Enter Born2beRoot username [default: ${DEFAULT_USER}]: " INPUT_USER
    B2BR_USER="${INPUT_USER:-$DEFAULT_USER}"
    NEED_EXPORT_HINT=1
fi

# User Password
if [ -z "${B2BR_PASSWORD:-}" ]; then
    read -s -p "Enter password for user '${B2BR_USER}': " B2BR_PASSWORD
    echo ""
    NEED_EXPORT_HINT=1
fi

# Root Password
if [ -z "${B2BR_ROOT_PASSWORD:-}" ]; then
    read -s -p "Enter root password: " B2BR_ROOT_PASSWORD
    echo ""
    NEED_EXPORT_HINT=1
fi

# SSH Host & Port
B2BR_HOST="${B2BR_HOST:-127.0.0.1}"
B2BR_PORT="${B2BR_PORT:-4242}"

# Display Export Hint if credentials were entered interactively
if [ "$NEED_EXPORT_HINT" -eq 1 ]; then
    echo -e "\n${DIM}Tip: You can export these variables in your shell to bypass prompts:${RESET}"
    echo -e "${DIM}export B2BR_USER=\"${B2BR_USER}\"${RESET}"
    echo -e "${DIM}export B2BR_PASSWORD=\"***\"${RESET}"
    echo -e "${DIM}export B2BR_ROOT_PASSWORD=\"***\"${RESET}"
    echo -e "${DIM}export B2BR_PORT=\"${B2BR_PORT}\"${RESET}"
    echo -e "${DIM}export B2BR_HOST=\"${B2BR_HOST}\"${RESET}\n"
fi

# Check sshpass requirement
if ! command -v sshpass >/dev/null 2>&1; then
    echo -e "${RED}Error: 'sshpass' is required to automate non-interactive SSH tests.${RESET}"
    echo "Install it using: sudo apt install -y sshpass"
    exit 1
fi

# ------------------------------------------------------------------------------
# 2. VirtualBox Port Forwarding Auto-Mapping (Port 4242)
# ------------------------------------------------------------------------------
section "VirtualBox Network & Port Mapping"

check_port() {
    nc -z -w 2 "$B2BR_HOST" "$B2BR_PORT" >/dev/null 2>&1 || \
    timeout 2 bash -c "</dev/tcp/${B2BR_HOST}/${B2BR_PORT}" >/dev/null 2>&1
}

if check_port; then
    record_result "PASS" "Port ${B2BR_PORT} is reachable on ${B2BR_HOST}"
else
    echo -e "  ${YELLOW}Port ${B2BR_PORT} is not responding on ${B2BR_HOST}.${RESET}"
    echo -e "  ${BLUE}Checking VirtualBox for active VM to add port mapping...${RESET}"

    DETECTED_VM=""
    if command -v VBoxManage >/dev/null 2>&1; then
        RUNNING_VMS=$(VBoxManage list runningvms 2>/dev/null || true)
        if [ -n "$RUNNING_VMS" ]; then
            DETECTED_VM=$(echo "$RUNNING_VMS" | head -n 1 | awk -F'"' '{print $2}')
        fi

        if [ -z "$DETECTED_VM" ]; then
            ALL_VMS=$(VBoxManage list vms 2>/dev/null || true)
            if [ -n "$ALL_VMS" ]; then
                DETECTED_VM=$(echo "$ALL_VMS" | grep -i "${B2BR_USER}" | head -n 1 | awk -F'"' '{print $2}' || true)
                if [ -z "$DETECTED_VM" ]; then
                    DETECTED_VM=$(echo "$ALL_VMS" | head -n 1 | awk -F'"' '{print $2}')
                fi
            fi
        fi

        if [ -n "$DETECTED_VM" ]; then
            echo -e "  Found VirtualBox VM: '${DETECTED_VM}'. Checking NAT port forwarding..."
            # Check if port rule exists
            HAS_RULE=$(VBoxManage showvminfo "$DETECTED_VM" 2>/dev/null | grep -i "${B2BR_PORT}" || true)
            if [ -z "$HAS_RULE" ]; then
                echo -e "  Adding rule 'SSH,tcp,,${B2BR_PORT},,${B2BR_PORT}' to '${DETECTED_VM}'..."
                if VBoxManage controlvm "$DETECTED_VM" natpf1 "SSH,tcp,,${B2BR_PORT},,${B2BR_PORT}" 2>/dev/null || \
                   VBoxManage modifyvm "$DETECTED_VM" --natpf1 "SSH,tcp,,${B2BR_PORT},,${B2BR_PORT}" 2>/dev/null; then
                    record_result "PASS" "Added NAT port forwarding rule (port ${B2BR_PORT}) to VM '${DETECTED_VM}'"
                    sleep 2
                else
                    record_result "WARN" "Failed to automatically add port forwarding to '${DETECTED_VM}'"
                fi
            else
                record_result "PASS" "NAT Port forwarding rule for port ${B2BR_PORT} already configured on '${DETECTED_VM}'"
            fi
        fi
    fi

    # Re-verify port
    if check_port; then
        record_result "PASS" "Port ${B2BR_PORT} is now reachable"
    else
        record_result "FAIL" "Cannot connect to port ${B2BR_PORT} on ${B2BR_HOST}" "Ensure the virtual machine is running with NAT port forwarding (Host: ${B2BR_PORT} -> Guest: ${B2BR_PORT})."
        echo -e "\n${RED}Aborting test suite: SSH service unreachable.${RESET}"
        exit 1
    fi
fi

# ------------------------------------------------------------------------------
# 3. SSH Execution Helper
# ------------------------------------------------------------------------------
run_ssh() {
    local cmd="$1"
    sshpass -p "$B2BR_PASSWORD" ssh -t -t -p "$B2BR_PORT" \
        -o StrictHostKeyChecking=no \
        -o UserKnownHostsFile=/dev/null \
        -o LogLevel=ERROR \
        -o ConnectTimeout=5 \
        "${B2BR_USER}@${B2BR_HOST}" "$cmd" 2>&1
}

run_sudo() {
    local cmd="$1"
    run_ssh "echo '${B2BR_PASSWORD}' | sudo -S bash -c '$cmd'"
}

# ------------------------------------------------------------------------------
# 4. SSH & User Authentication
# ------------------------------------------------------------------------------
section "SSH & Authentication Checks"

# Test standard user login
SSH_TEST=$(run_ssh "echo 'SSH_OK'" || echo "SSH_FAILED")
if echo "$SSH_TEST" | grep -q "SSH_OK"; then
    record_result "PASS" "SSH connection with user '${B2BR_USER}' on port ${B2BR_PORT}"
else
    record_result "FAIL" "SSH connection failed for '${B2BR_USER}'" "$SSH_TEST"
    exit 1
fi

# Test root login over SSH (must be rejected)
ROOT_SSH_TEST=$(sshpass -p "$B2BR_ROOT_PASSWORD" ssh -p "$B2BR_PORT" \
    -o StrictHostKeyChecking=no \
    -o UserKnownHostsFile=/dev/null \
    -o LogLevel=ERROR \
    -o ConnectTimeout=3 \
    "root@${B2BR_HOST}" "echo 'ROOT_SSH_ALLOWED'" 2>&1 || echo "ROOT_BLOCKED")

if echo "$ROOT_SSH_TEST" | grep -q "ROOT_SSH_ALLOWED"; then
    record_result "FAIL" "PermitRootLogin is enabled over SSH (Must be disabled!)"
else
    record_result "PASS" "Root login disabled over SSH (PermitRootLogin no)"
fi

# ------------------------------------------------------------------------------
# 5. Operating System & GUI
# ------------------------------------------------------------------------------
section "Operating System & Headless Setup"

OS_NAME=$(run_ssh "grep '^PRETTY_NAME=' /etc/os-release | cut -d'=' -f2 | tr -d '\"'")
if echo "$OS_NAME" | grep -qiE "Debian|Rocky"; then
    record_result "PASS" "Distribution matches subject requirements: $OS_NAME"
else
    record_result "WARN" "Distribution is not Debian or Rocky Linux: $OS_NAME"
fi

DEFAULT_TARGET=$(run_ssh "systemctl get-default" | tr -d '\r\n')
if [ "$DEFAULT_TARGET" = "multi-user.target" ]; then
    record_result "PASS" "Default systemd target is headless (multi-user.target)"
else
    record_result "FAIL" "Default systemd target is '$DEFAULT_TARGET' (Expected: multi-user.target)"
fi

GUI_CHECK=$(run_ssh "dpkg -l 2>/dev/null | grep -E 'xserver-xorg|gnome-core|xfce4|lightdm|gdm3|sddm' || true")
if [ -z "$GUI_CHECK" ]; then
    record_result "PASS" "No X11 / Wayland desktop environment or display manager installed"
else
    record_result "WARN" "Possible GUI packages detected" "$GUI_CHECK"
fi

# ------------------------------------------------------------------------------
# 6. Hostname, Users & Groups
# ------------------------------------------------------------------------------
section "Hostname & User Groups"

ACTUAL_HOSTNAME=$(run_ssh "hostname" | tr -d '\r\n')
EXPECTED_HOSTNAME="${B2BR_USER}42"

if [ "$ACTUAL_HOSTNAME" = "$EXPECTED_HOSTNAME" ] || [[ "$ACTUAL_HOSTNAME" =~ 42$ ]]; then
    record_result "PASS" "Hostname conforms to subject: '$ACTUAL_HOSTNAME'"
else
    record_result "FAIL" "Hostname is '$ACTUAL_HOSTNAME' (Expected: '${EXPECTED_HOSTNAME}')"
fi

USER_GROUPS=$(run_ssh "id -nG ${B2BR_USER}" | tr -d '\r\n')
if echo "$USER_GROUPS" | grep -q "sudo" && echo "$USER_GROUPS" | grep -q "user42"; then
    record_result "PASS" "User '${B2BR_USER}' belongs to both 'sudo' and 'user42' groups" "Groups: $USER_GROUPS"
else
    record_result "FAIL" "User '${B2BR_USER}' missing required groups" "Current: $USER_GROUPS (Must include sudo and user42)"
fi

# ------------------------------------------------------------------------------
# 7. Memory & Partitioning (LVM over LUKS)
# ------------------------------------------------------------------------------
section "Partitioning, LVM & Encryption"

# RAM check
TOTAL_RAM_MB=$(run_ssh "free -m | awk '/^Mem:/{print \$2}'" | tr -d '\r\n')
if [ "$TOTAL_RAM_MB" -ge 900 ] && [ "$TOTAL_RAM_MB" -le 1150 ]; then
    record_result "PASS" "RAM size configured around 1024 MB (${TOTAL_RAM_MB} MB)"
else
    record_result "WARN" "RAM size is ${TOTAL_RAM_MB} MB (Expected ~1024 MB)"
fi

# SWAP check
TOTAL_SWAP_MB=$(run_ssh "free -m | awk '/^Swap:/{print \$2}'" | tr -d '\r\n')
if [ "$TOTAL_SWAP_MB" -ge 900 ] && [ "$TOTAL_SWAP_MB" -le 1150 ]; then
    record_result "PASS" "Swap size configured around 1024 MB (${TOTAL_SWAP_MB} MB)"
else
    record_result "WARN" "Swap size is ${TOTAL_SWAP_MB} MB (Expected ~1024 MB)"
fi

# LUKS Encryption
LSBLK_OUT=$(run_ssh "lsblk -o NAME,TYPE,MOUNTPOINTS")
if echo "$LSBLK_OUT" | grep -q "crypt"; then
    record_result "PASS" "Encrypted volume detected (LUKS / crypt)"
else
    record_result "FAIL" "No encrypted (crypt) partition found in lsblk"
fi

# Unencrypted /boot
BOOT_LINE=$(echo "$LSBLK_OUT" | grep -E "/boot$" || true)
if [ -n "$BOOT_LINE" ] && ! echo "$BOOT_LINE" | grep -q "crypt"; then
    record_result "PASS" "/boot is mounted on dedicated unencrypted partition"
else
    record_result "WARN" "Check /boot partition layout: $BOOT_LINE"
fi

# LVM verification
LVM_COUNT=$(run_ssh "pvs 2>/dev/null | wc -l" | tr -d '\r\n')
LV_COUNT=$(run_ssh "lvs 2>/dev/null | wc -l" | tr -d '\r\n')
if [ "$LVM_COUNT" -gt 1 ] && [ "$LV_COUNT" -ge 3 ]; then
    record_result "PASS" "LVM active with at least 2 Logical Volumes" "Detected $(($LV_COUNT - 1)) Logical Volumes"
else
    record_result "FAIL" "LVM not properly configured or insufficient Logical Volumes"
fi

# ------------------------------------------------------------------------------
# 8. Sudo Security Policy
# ------------------------------------------------------------------------------
section "Sudo Security Policy"

SUDO_CONFIG=$(run_sudo "cat /etc/sudoers /etc/sudoers.d/* 2>/dev/null")

# passwd_tries=3
if echo "$SUDO_CONFIG" | grep -Eq "passwd_tries\s*=\s*3"; then
    record_result "PASS" "Sudo password tries limited to 3 (passwd_tries=3)"
else
    record_result "FAIL" "Missing 'Defaults passwd_tries=3'"
fi

# badpass_message
if echo "$SUDO_CONFIG" | grep -Eq "badpass_message\s*="; then
    record_result "PASS" "Custom bad password message configured (badpass_message)"
else
    record_result "FAIL" "Missing custom 'Defaults badpass_message=...'"
fi

# logfile
if echo "$SUDO_CONFIG" | grep -Eq "logfile\s*=\s*\"?/var/log/sudo"; then
    record_result "PASS" "Sudo logging directed to /var/log/sudo/"
else
    record_result "FAIL" "Missing 'Defaults logfile=\"/var/log/sudo/...\"'"
fi

# log_input and log_output
if echo "$SUDO_CONFIG" | grep -q "log_input" && echo "$SUDO_CONFIG" | grep -q "log_output"; then
    record_result "PASS" "Sudo input and output archiving enabled (log_input, log_output)"
else
    record_result "FAIL" "Missing 'Defaults log_input, log_output'"
fi

# requiretty
if echo "$SUDO_CONFIG" | grep -q "requiretty"; then
    record_result "PASS" "TTY mode enforced for sudo (requiretty)"
else
    record_result "FAIL" "Missing 'Defaults requiretty'"
fi

# secure_path
if echo "$SUDO_CONFIG" | grep -q "secure_path"; then
    record_result "PASS" "Sudo secure_path restricted"
else
    record_result "FAIL" "Missing 'Defaults secure_path=...'"
fi

# Verify log file exists on disk
SUDO_LOG_CHECK=$(run_sudo "[ -d /var/log/sudo ] && ls -la /var/log/sudo/ | grep -E '\.log' || true")
if [ -n "$SUDO_LOG_CHECK" ]; then
    record_result "PASS" "Sudo log file created in /var/log/sudo/"
else
    record_result "WARN" "No log files found in /var/log/sudo/ yet"
fi

# ------------------------------------------------------------------------------
# 9. UFW Firewall
# ------------------------------------------------------------------------------
section "UFW Firewall"

UFW_STATUS=$(run_sudo "ufw status verbose")

if echo "$UFW_STATUS" | grep -qi "Status: active"; then
    record_result "PASS" "UFW firewall is active"
else
    record_result "FAIL" "UFW firewall is not active"
fi

if echo "$UFW_STATUS" | grep -qi "Default: deny (incoming)"; then
    record_result "PASS" "Default incoming policy is deny"
else
    record_result "FAIL" "Default incoming policy is not deny"
fi

if echo "$UFW_STATUS" | grep -Eq "4242.*ALLOW IN"; then
    record_result "PASS" "Port 4242/tcp explicitly allowed in UFW"
else
    record_result "FAIL" "Port 4242 is not allowed in UFW"
fi

# Check no unauthorized open ports
OTHER_PORTS=$(echo "$UFW_STATUS" | grep -E "ALLOW IN" | grep -v "4242" || true)
if [ -z "$OTHER_PORTS" ]; then
    record_result "PASS" "Only port 4242 is allowed (No extraneous ports open)"
else
    record_result "WARN" "Additional ports allowed in UFW" "$OTHER_PORTS"
fi

# ------------------------------------------------------------------------------
# 10. SSH Configuration
# ------------------------------------------------------------------------------
section "SSH Daemon Configuration"

SSHD_CONFIG=$(run_sudo "grep -v '^#' /etc/ssh/sshd_config /etc/ssh/sshd_config.d/* 2>/dev/null || true")

if echo "$SSHD_CONFIG" | grep -Eq "Port\s+4242"; then
    record_result "PASS" "SSH daemon configured solely on port 4242"
else
    record_result "FAIL" "SSH daemon Port 4242 directive missing"
fi

if echo "$SSHD_CONFIG" | grep -Eq "PermitRootLogin\s+no"; then
    record_result "PASS" "PermitRootLogin no verified in sshd_config"
else
    record_result "FAIL" "PermitRootLogin no missing in sshd_config"
fi

# ------------------------------------------------------------------------------
# 11. Password Policy (Aging & Quality)
# ------------------------------------------------------------------------------
section "Password Policies"

LOGIN_DEFS=$(run_sudo "grep -E '^PASS_(MAX|MIN|WARN)' /etc/login.defs")

if echo "$LOGIN_DEFS" | grep -Eq "PASS_MAX_DAYS\s+30"; then
    record_result "PASS" "PASS_MAX_DAYS set to 30 in /etc/login.defs"
else
    record_result "FAIL" "PASS_MAX_DAYS is not 30 in /etc/login.defs"
fi

if echo "$LOGIN_DEFS" | grep -Eq "PASS_MIN_DAYS\s+2"; then
    record_result "PASS" "PASS_MIN_DAYS set to 2 in /etc/login.defs"
else
    record_result "FAIL" "PASS_MIN_DAYS is not 2 in /etc/login.defs"
fi

if echo "$LOGIN_DEFS" | grep -Eq "PASS_WARN_AGE\s+7"; then
    record_result "PASS" "PASS_WARN_AGE set to 7 in /etc/login.defs"
else
    record_result "FAIL" "PASS_WARN_AGE is not 7 in /etc/login.defs"
fi

# Account Aging Check
CHAGE_OUT=$(run_sudo "chage -l ${B2BR_USER}")
if echo "$CHAGE_OUT" | grep -q "30" && echo "$CHAGE_OUT" | grep -q "2" && echo "$CHAGE_OUT" | grep -q "7"; then
    record_result "PASS" "Password expiration parameters applied to '${B2BR_USER}'"
else
    record_result "WARN" "Password expiration may not be applied to '${B2BR_USER}'" "$CHAGE_OUT"
fi

# libpam-pwquality parameters
PAM_PWQUALITY=$(run_sudo "grep -E 'pam_pwquality\.so' /etc/pam.d/common-password /etc/pam.d/system-auth 2>/dev/null || true")

if [ -n "$PAM_PWQUALITY" ]; then
    record_result "PASS" "pam_pwquality module is configured"
    
    for rule in "minlen=10" "ucredit=-1" "lcredit=-1" "dcredit=-1" "maxrepeat=3" "reject_user" "difok=7" "enforce_for_root"; do
        if echo "$PAM_PWQUALITY" | grep -q "$rule"; then
            record_result "PASS" "Password complexity rule '$rule' present"
        else
            record_result "FAIL" "Missing password complexity rule '$rule'"
        fi
    done
else
    record_result "FAIL" "libpam-pwquality not found in PAM configuration"
fi

# ------------------------------------------------------------------------------
# 12. Monitoring Script & Cron
# ------------------------------------------------------------------------------
section "Monitoring Script & Cron"

SCRIPT_PATH="/usr/local/bin/monitoring.sh"
SCRIPT_EXISTS=$(run_sudo "[ -x $SCRIPT_PATH ] && echo 'EXISTS' || echo 'MISSING'")

if [ "$SCRIPT_EXISTS" = "EXISTS" ]; then
    record_result "PASS" "Monitoring script exists and is executable at $SCRIPT_PATH"
else
    record_result "FAIL" "Monitoring script missing or not executable at $SCRIPT_PATH"
fi

CRON_CHECK=$(run_sudo "crontab -l 2>/dev/null | grep -v '^#' || true")
if echo "$CRON_CHECK" | grep -Eq "\*/10\s+\*\s+\*\s+\*\s+\*.*monitoring\.sh"; then
    record_result "PASS" "Cron scheduled every 10 minutes in root crontab"
else
    record_result "FAIL" "Cron entry '*/10 * * * * ... monitoring.sh' missing from root crontab"
fi

# Run monitoring script and evaluate output
MONITOR_OUT=$(run_sudo "sed 's/wall << BANNER/cat << BANNER/' $SCRIPT_PATH | bash" || true)

REQUIRED_FIELDS=(
    "#Architecture"
    "#Physical CPU"
    "#vCPU"
    "#Memory Usage"
    "#Disk Usage"
    "#CPU load"
    "#Last boot"
    "#LVM use"
    "#TCP Connections"
    "#User log"
    "#Network"
    "#Sudo"
)

ALL_FIELDS_OK=1
for field in "${REQUIRED_FIELDS[@]}"; do
    if echo "$MONITOR_OUT" | grep -qi "$field"; then
        continue
    else
        ALL_FIELDS_OK=0
        record_result "FAIL" "Monitoring script missing field: $field"
    fi
done

if [ "$ALL_FIELDS_OK" -eq 1 ]; then
    record_result "PASS" "Monitoring script outputs all 12 required telemetry fields"
fi

# Check CPU load bug
if echo "$MONITOR_OUT" | grep -qi "#CPU load: 100.0%"; then
    record_result "WARN" "CPU load is reported at 100.0% (check for top awk delimiter bug)"
fi

# ------------------------------------------------------------------------------
# 13. Virtual Disk Signature Check (Optional)
# ------------------------------------------------------------------------------
if [ -n "${B2BR_DISK:-}" ] && [ -f "$B2BR_DISK" ]; then
    section "Disk Signature Verification"
    SIG_FILE="${B2BR_SIGNATURE_FILE:-signature.txt}"
    if [ -f "$SIG_FILE" ]; then
        EXPECTED_HASH=$(awk '{print $1}' "$SIG_FILE" | grep -E '^[0-9a-fA-F]{40}$' | head -n 1)
        ACTUAL_HASH=$(sha1sum "$B2BR_DISK" | awk '{print $1}')
        if [ "$EXPECTED_HASH" = "$ACTUAL_HASH" ]; then
            record_result "PASS" "Virtual disk SHA1 signature matches $SIG_FILE"
        else
            record_result "FAIL" "SHA1 mismatch: disk ($ACTUAL_HASH) vs signature.txt ($EXPECTED_HASH)"
        fi
    fi
fi

# ------------------------------------------------------------------------------
# Final Score Summary
# ------------------------------------------------------------------------------
echo -e "\n${BOLD}${CYAN}==================================================================${RESET}"
echo -e "                   ${BOLD}AUDIT SUMMARY & RESULTS${RESET}"
echo -e "${CYAN}==================================================================${RESET}"
echo -e "  Total Tests:    ${BOLD}${TOTAL_TESTS}${RESET}"
echo -e "  Passed:         ${GREEN}${BOLD}${PASSED_TESTS}${RESET}"
echo -e "  Failed:         ${RED}${BOLD}${FAILED_TESTS}${RESET}"
echo -e "  Warnings:       ${YELLOW}${BOLD}${WARNING_TESTS}${RESET}"
echo -e "${CYAN}==================================================================${RESET}"

if [ "$FAILED_TESTS" -eq 0 ]; then
    echo -e "\n  ${GREEN}${BOLD}🎉 CONGRATULATIONS! Born2beRoot VM is 100% compliant with 42 guidelines.${RESET}\n"
    exit 0
else
    echo -e "\n  ${RED}${BOLD}❌ Some requirements failed. Please review the failed items above.${RESET}\n"
    exit 1
fi
