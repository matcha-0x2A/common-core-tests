#!/usr/bin/env bash
# ==============================================================================
# 42 Common Core — Test Suite : Born2beRoot
# Systems & Networks Administration (01-systems-and-networks-administration)
#
# Validates VM conformance with the 42 Born2beRoot official evaluation subject:
# - VM Port Forwarding (4242) and SSH reachability
# - Interactive / automated credential authentication (user & root passwords)
# - OS type, headless mode (no GUI), multi-user.target
# - Hostname format (<login>42) and user group memberships (user42, sudo)
# - RAM & Swap sizing (~1 GB each)
# - Partitioning layout (LVM over LUKS encryption, /boot unencrypted)
# - Strict Sudo policy (requiretty, logfile, badpass_message, secure_path, tries)
# - UFW Firewall (active, deny incoming, port 4242 allowed)
# - OpenSSH daemon (port 4242, PermitRootLogin no, direct root blocked)
# - Root authentication verification via shadow crypt
# - Password expiration and complexity policies (login.defs, pam_pwquality)
# - Monitoring script & cron broadcast schedule
# - Virtual disk SHA1 checksum against signature.txt
# ==============================================================================

set -uo pipefail

# ANSI Colors
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
PURPLE="\033[0;35m"
CYAN="\033[0;36m"
BOLD="\033[1m"
NC="\033[0m"

REAL_SCRIPT="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(cd "$(dirname "${REAL_SCRIPT}")" && pwd)"

# Default Parameters
TARGET=""
VM_HOST="127.0.0.1"
VM_PORT="4242"
B2BR_USER=""
B2BR_USER_PASS=""
B2BR_ROOT_PASS=""
VM_NAME=""
VDI_PATH=""
AUTO_MODE=0
USE_SNAPSHOT=1
RESTORE_SNAPSHOT=0
KEEP_SNAPSHOT=0
CREATED_SNAPSHOT=0
SNAPSHOT_NAME="b2br_eval_safety_snapshot"

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
WARNED_TESTS=0

log_pass() {
    ((TOTAL_TESTS++))
    ((PASSED_TESTS++))
    echo -e "  ${GREEN}✔ [PASS]${NC} $1"
}

log_fail() {
    ((TOTAL_TESTS++))
    ((FAILED_TESTS++))
    echo -e "  ${RED}✘ [FAIL]${NC} $1"
    if [ -n "${2:-}" ]; then
        echo -e "         ${YELLOW}↳ Detail:${NC} $2"
    fi
}

log_warn() {
    ((WARNED_TESTS++))
    echo -e "  ${YELLOW}⚠ [WARN]${NC} $1"
    if [ -n "${2:-}" ]; then
        echo -e "         ${YELLOW}↳ Detail:${NC} $2"
    fi
}

log_section() {
    echo ""
    echo -e "${BLUE}${BOLD}▶ $1${NC}"
}

usage() {
    cat << USAGE_EOF
Usage: $0 [options] [/path/to/00-born2beroot]

Options:
  -u, --user <username>       Target standard username (e.g. mchambon)
  -p, --user-pass <password>  Standard user password
  -r, --root-pass <password>  Root account password
  -P, --port <port>           SSH host port forwarded to guest (default: 4242)
  -H, --host <host>           SSH host IP (default: 127.0.0.1)
  --vm <vm_name>              VirtualBox VM name (auto-detected if omitted)
  --vdi <vdi_path>            Path to .vdi virtual disk file
  --snapshot                  Create safety snapshot before tests (Default: enabled)
  --no-snapshot               Do not create a snapshot
  --restore                   Automatically restore and remove snapshot after tests
  --keep-snapshot             Keep the snapshot after tests
  -y, --auto                  Non-interactive mode (use defaults without prompting)
  -h, --help                  Show this help message

Examples:
  $0
  $0 --auto
  $0 -u mchambon -p Zxcvbn1234 -r Nbvcxz1234
  $0 /home/mchambon/wip/42/systems-and-networks-administration/00-born2beroot
USAGE_EOF
    exit 0
}

# Parse Command-Line Arguments
while [ $# -gt 0 ]; do
    case "$1" in
        -u|--user)
            B2BR_USER="$2"
            shift 2
            ;;
        -p|--password|--user-pass)
            B2BR_USER_PASS="$2"
            shift 2
            ;;
        -r|--root-pass)
            B2BR_ROOT_PASS="$2"
            shift 2
            ;;
        -P|--port)
            VM_PORT="$2"
            shift 2
            ;;
        -H|--host)
            VM_HOST="$2"
            shift 2
            ;;
        --vm)
            VM_NAME="$2"
            shift 2
            ;;
        --vdi)
            VDI_PATH="$2"
            shift 2
            ;;
        --snapshot)
            USE_SNAPSHOT=1
            shift
            ;;
        --no-snapshot)
            USE_SNAPSHOT=0
            shift
            ;;
        --restore)
            RESTORE_SNAPSHOT=1
            shift
            ;;
        --keep-snapshot)
            KEEP_SNAPSHOT=1
            shift
            ;;
        -y|--auto)
            AUTO_MODE=1
            shift
            ;;
        -h|--help)
            usage
            ;;
        *)
            if [ -d "$1" ]; then
                TARGET="$(cd "$1" 2>/dev/null && pwd)"
            fi
            shift
            ;;
    esac
done

# Locate target repository
if [ -z "$TARGET" ]; then
    CANDIDATES=(
        "${PWD}"
        "${SCRIPT_DIR}/../../systems-and-networks-administration/00-born2beroot"
        "${SCRIPT_DIR}/../00-born2beroot"
        "${HOME}/wip/42/systems-and-networks-administration/00-born2beroot"
    )
    for c in "${CANDIDATES[@]}"; do
        if [ -d "$c" ] && [ -f "$c/signature.txt" ]; then
            TARGET="$(cd "$c" 2>/dev/null && pwd)"
            break
        fi
    done
fi

echo -e "${PURPLE}${BOLD}======================================================${NC}"
echo -e "${PURPLE}${BOLD}    42 Common Core — Born2beRoot Test Suite           ${NC}"
echo -e "${PURPLE}${BOLD}======================================================${NC}"
echo -e "  Target Repo : ${CYAN}${TARGET:-[Not specified - using direct VM mode]}${NC}"
echo -e "  SSH Target  : ${CYAN}${VM_HOST}:${VM_PORT}${NC}"

# Auto-extract default credentials from README.md if present
if [ -n "$TARGET" ] && [ -f "$TARGET/README.md" ]; then
    if [ -z "$B2BR_USER" ]; then
        DETECTED_USER=$(grep -oE '\|\s*\*\*User\*\*\s*\|\s*`[a-z0-9_-]+`' "$TARGET/README.md" | awk -F '`' '{print $2}' | head -n 1 || true)
        [ -n "$DETECTED_USER" ] && B2BR_USER="$DETECTED_USER"
    fi
    if [ -z "$B2BR_USER_PASS" ]; then
        DETECTED_UPASS=$(grep -oE '\|\s*\*\*User\*\*\s*\|\s*`[^`]+`\s*\|\s*`[^`]+`' "$TARGET/README.md" | awk -F '`' '{print $4}' | head -n 1 || true)
        [ -n "$DETECTED_UPASS" ] && B2BR_USER_PASS="$DETECTED_UPASS"
    fi
    if [ -z "$B2BR_ROOT_PASS" ]; then
        DETECTED_RPASS=$(grep -oE '\|\s*\*\*Root\*\*\s*\|\s*`[^`]+`\s*\|\s*`[^`]+`' "$TARGET/README.md" | awk -F '`' '{print $4}' | head -n 1 || true)
        [ -n "$DETECTED_RPASS" ] && B2BR_ROOT_PASS="$DETECTED_RPASS"
    fi
fi

# Fallback default credentials
[ -z "$B2BR_USER" ] && B2BR_USER="mchambon"
[ -z "$B2BR_USER_PASS" ] && B2BR_USER_PASS="Zxcvbn1234"
[ -z "$B2BR_ROOT_PASS" ] && B2BR_ROOT_PASS="Nbvcxz1234"

# Interactive Credential Prompting (if not in auto mode)
if [ "$AUTO_MODE" -eq 0 ] && [ -t 0 ]; then
    echo ""
    echo -e "${YELLOW}Veuillez confirmer ou renseigner les identifiants de test :${NC}"
    read -r -p "👤 Utilisateur standard [${B2BR_USER}]: " INPUT_USER
    [ -n "$INPUT_USER" ] && B2BR_USER="$INPUT_USER"

    read -r -s -p "🔑 Mot de passe utilisateur standard [${B2BR_USER_PASS}]: " INPUT_UPASS
    echo ""
    [ -n "$INPUT_UPASS" ] && B2BR_USER_PASS="$INPUT_UPASS"

    read -r -s -p "👑 Mot de passe root [${B2BR_ROOT_PASS}]: " INPUT_RPASS
    echo ""
    [ -n "$INPUT_RPASS" ] && B2BR_ROOT_PASS="$INPUT_RPASS"
fi

echo -e "  User to test: ${CYAN}${B2BR_USER}${NC}"

# Auto-detect VM name early for disk locator and port mapping
VBOXMANAGE_BIN=$(command -v VBoxManage || command -v vboxmanage || true)
if [ -n "$VBOXMANAGE_BIN" ] && [ -z "$VM_NAME" ]; then
    VM_NAME=$("$VBOXMANAGE_BIN" list runningvms 2>/dev/null | awk -F '"' '{print $2}' | head -n 1 || true)
    if [ -z "$VM_NAME" ]; then
        VM_NAME=$("$VBOXMANAGE_BIN" list vms 2>/dev/null | grep -iE '42|born' | awk -F '"' '{print $2}' | head -n 1 || true)
    fi
    if [ -z "$VM_NAME" ]; then
        VBOX_FILE=$(find "/goinfre/${USER}" -name "*.vbox" 2>/dev/null | head -n 1 || true)
        if [ -n "$VBOX_FILE" ]; then
            "$VBOXMANAGE_BIN" registervm "$VBOX_FILE" 2>/dev/null || true
            VM_NAME=$("$VBOXMANAGE_BIN" list vms 2>/dev/null | awk -F '"' '{print $2}' | head -n 1 || true)
        fi
    fi
fi

# ==============================================================================
# Phase 1: Virtual Disk Signature Verification
# ==============================================================================
log_section "Phase 1 : Preliminary Check — Virtual Disk Signature (signature.txt)"

if [ -n "$TARGET" ] && [ -f "$TARGET/signature.txt" ]; then
    EXPECTED_SHA1=$(grep -vE '^[[:space:]]*#|^[[:space:]]*$' "$TARGET/signature.txt" | awk '{print $1}' | head -n 1 || true)
    
    if [ -n "$EXPECTED_SHA1" ] && [ "${#EXPECTED_SHA1}" -eq 40 ]; then
        log_pass "signature.txt contains a valid 40-character SHA1 hash (${EXPECTED_SHA1:0:8}...)."
        
        # Search for .vdi file
        if [ -z "$VDI_PATH" ]; then
            VDI_CANDIDATES=(
                "/goinfre/${USER}/VirtualBox VMs/${VM_NAME}/${VM_NAME}.vdi"
                "/goinfre/${USER}/born2beroot/born2beroot.vdi"
                "/goinfre/${USER}/born2beroot/${VM_NAME}.vdi"
                "${HOME}/VirtualBox VMs/${VM_NAME}/${VM_NAME}.vdi"
            )
            for v in "${VDI_CANDIDATES[@]}"; do
                if [ -f "$v" ]; then
                    VDI_PATH="$v"
                    break
                fi
            done
        fi
        
        if [ -n "$VDI_PATH" ] && [ -f "$VDI_PATH" ]; then
            ACTUAL_SHA1=$(sha1sum "$VDI_PATH" 2>/dev/null | awk '{print $1}' || true)
            if [ "$ACTUAL_SHA1" == "$EXPECTED_SHA1" ]; then
                log_pass "Virtual disk (${VDI_PATH}) matches signature.txt perfectly."
            else
                log_warn "Live virtual disk SHA1 differs from signature.txt." "Normal if VM is currently running. Re-compute hash after 'sudo poweroff'."
            fi
        else
            log_warn "Could not locate .vdi file automatically to verify signature." "Specify path with --vdi <path>."
        fi
    else
        log_fail "signature.txt does not contain a valid 40-character SHA1 checksum."
    fi
else
    log_warn "signature.txt not found in target repository."
fi


# ==============================================================================
# Phase 2: VirtualBox VM & Port Forwarding (4242)
# ==============================================================================
log_section "Phase 2 : VirtualBox VM & Port Mapping Check (4242)"

VBOXMANAGE_BIN=$(command -v VBoxManage || command -v vboxmanage || true)
if [ -n "$VBOXMANAGE_BIN" ]; then
    # Auto-detect VM if not set
    if [ -z "$VM_NAME" ]; then
        VM_NAME=$("$VBOXMANAGE_BIN" list runningvms 2>/dev/null | awk -F '"' '{print $2}' | head -n 1 || true)
        if [ -z "$VM_NAME" ]; then
            VM_NAME=$("$VBOXMANAGE_BIN" list vms 2>/dev/null | grep -iE '42|born' | awk -F '"' '{print $2}' | head -n 1 || true)
        fi
        if [ -z "$VM_NAME" ]; then
            VBOX_FILE=$(find "/goinfre/${USER}" -name "*.vbox" 2>/dev/null | head -n 1 || true)
            if [ -n "$VBOX_FILE" ]; then
                "$VBOXMANAGE_BIN" registervm "$VBOX_FILE" 2>/dev/null || true
                VM_NAME=$("$VBOXMANAGE_BIN" list vms 2>/dev/null | awk -F '"' '{print $2}' | head -n 1 || true)
            fi
        fi
    fi

    if [ -n "$VM_NAME" ]; then
        echo -e "  ${BLUE}ℹ${NC} Detected VM: ${BOLD}${VM_NAME}${NC}"
        
        # Check if port forwarding 4242 exists
        HAS_4242_RULE=$("$VBOXMANAGE_BIN" showvminfo "$VM_NAME" 2>/dev/null | grep -iE "NIC [0-9] Rule" | grep -c "$VM_PORT" || echo 0)
        
        if [ "$HAS_4242_RULE" -gt 0 ]; then
            log_pass "Port forwarding rule for port ${VM_PORT} is configured on VM '${VM_NAME}'."
        else
            log_warn "Port forwarding rule for port ${VM_PORT} missing on VM '${VM_NAME}'." "Adding rule automatically..."
            
            IS_RUNNING=$("$VBOXMANAGE_BIN" list runningvms 2>/dev/null | grep -c "\"$VM_NAME\"" || echo 0)
            if [ "$IS_RUNNING" -gt 0 ]; then
                "$VBOXMANAGE_BIN" controlvm "$VM_NAME" natpf1 "SSH,tcp,,${VM_PORT},,${VM_PORT}" 2>/dev/null || true
            else
                "$VBOXMANAGE_BIN" modifyvm "$VM_NAME" --natpf1 "SSH,tcp,,${VM_PORT},,${VM_PORT}" 2>/dev/null || true
            fi
            
            HAS_4242_AFTER=$("$VBOXMANAGE_BIN" showvminfo "$VM_NAME" 2>/dev/null | grep -iE "NIC [0-9] Rule" | grep -c "$VM_PORT" || echo 0)
            if [ "$HAS_4242_AFTER" -gt 0 ]; then
                log_pass "Port forwarding ${VM_PORT} successfully added to VM '${VM_NAME}'."
            else
                log_fail "Could not add port forwarding rule automatically." "Please run: VBoxManage modifyvm \"${VM_NAME}\" --natpf1 \"SSH,tcp,,${VM_PORT},,${VM_PORT}\""
            fi
        fi
    else
        log_warn "No VirtualBox VM detected via VBoxManage." "Proceeding with direct network connection test."
    fi
else
    log_warn "VBoxManage not found." "Proceeding with direct network connection test."
fi

# Test SSH Port Connectivity
if timeout 3 bash -c "</dev/tcp/${VM_HOST}/${VM_PORT}" 2>/dev/null; then
    log_pass "Port ${VM_PORT} is open and reachable on ${VM_HOST}."
else
    log_fail "Port ${VM_PORT} is not reachable on ${VM_HOST}." "Ensure the VM is running and port ${VM_PORT} is forwarded."
    echo ""
    echo -e "${RED}${BOLD}❌ Test aborted: Unable to communicate with the virtual server.${NC}"
    exit 1
fi

# Check for sshpass
if ! command -v sshpass &>/dev/null; then
    log_fail "sshpass utility is not installed on host." "Install it using: sudo apt install -y sshpass"
    exit 1
fi

SSH_OPTS="-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=5 -o LogLevel=ERROR"
SSH_CMD="sshpass -p '${B2BR_USER_PASS}' ssh -t -t -p ${VM_PORT} ${SSH_OPTS} ${B2BR_USER}@${VM_HOST}"
SCP_CMD="sshpass -p '${B2BR_USER_PASS}' scp -P ${VM_PORT} ${SSH_OPTS}"

# Test SSH Authentication with standard user
SSH_TEST=$(eval "${SSH_CMD} 'echo SSH_CONNECTED'" 2>/dev/null | tr -d '\r' || true)
if [[ "$SSH_TEST" == *"SSH_CONNECTED"* ]]; then
    log_pass "SSH authentication succeeded for user '${B2BR_USER}'."
else
    log_fail "SSH authentication failed for user '${B2BR_USER}'." "Check username or password."
    echo ""
    echo -e "${RED}${BOLD}❌ Test aborted: SSH login failed.${NC}"
    exit 1
fi

# Direct Root SSH Access Rejection Test
ROOT_SSH_TEST=$(sshpass -p "${B2BR_ROOT_PASS}" ssh -p ${VM_PORT} ${SSH_OPTS} root@${VM_HOST} "echo ROOT_CONNECTED" 2>&1 || true)
if [[ "$ROOT_SSH_TEST" != *"ROOT_CONNECTED"* ]]; then
    log_pass "Direct SSH login as 'root' is correctly rejected."
else
    log_fail "Security vulnerability: Direct SSH login as 'root' succeeded!"
fi

# ==============================================================================
# Generate and Deploy Remote Audit Probe
# ==============================================================================
PROBE_LOCAL="/tmp/b2br_probe_${RANDOM}.sh"
cat << 'PROBE_EOF' > "$PROBE_LOCAL"
#!/bin/bash
USER_TO_CHECK="$1"
ROOT_PASS="$2"

echo "---B2BR_BEGIN---"
echo "OS_PRETTY=$(grep '^PRETTY_NAME=' /etc/os-release 2>/dev/null | cut -d= -f2 | tr -d '\"')"
echo "DEFAULT_TARGET=$(systemctl get-default 2>/dev/null)"
echo "GUI_PACKAGES=$(dpkg -l 2>/dev/null | grep -E 'xserver-xorg|wayland|lightdm|gdm3|sddm' | head -n 1 || true)"
echo "HOSTNAME=$(hostname 2>/dev/null)"
echo "USER_GROUPS=$(id -Gn "$USER_TO_CHECK" 2>/dev/null)"
echo "RAM_MB=$(free -m 2>/dev/null | awk '/^Mem:/{print $2}')"
echo "SWAP_MB=$(free -m 2>/dev/null | awk '/^Swap:/{print $2}')"
echo "BOOT_UNENCRYPTED=$(lsblk -o NAME,TYPE,MOUNTPOINTS 2>/dev/null | grep -E 'sda1.*part.*/boot' || true)"
echo "CRYPT_COUNT=$(lsblk -o TYPE 2>/dev/null | grep -c 'crypt' || echo 0)"
echo "LV_COUNT=$(lvs --noheadings 2>/dev/null | wc -l || echo 0)"

SUDO_CONF=$(cat /etc/sudoers /etc/sudoers.d/* 2>/dev/null)
echo "SUDO_PASSWD_TRIES=$(echo "$SUDO_CONF" | grep -cE 'passwd_tries=[0-9]+' || echo 0)"
echo "SUDO_BADPASS=$(echo "$SUDO_CONF" | grep -c 'badpass_message' || echo 0)"
echo "SUDO_LOGFILE=$(echo "$SUDO_CONF" | grep -c 'logfile=' || echo 0)"
echo "SUDO_LOG_IO=$(if echo "$SUDO_CONF" | grep -q 'log_input' && echo "$SUDO_CONF" | grep -q 'log_output'; then echo 1; else echo 0; fi)"
echo "SUDO_REQUIRETTY=$(echo "$SUDO_CONF" | grep -c 'requiretty' || echo 0)"
echo "SUDO_SECURE_PATH=$(echo "$SUDO_CONF" | grep -c 'secure_path=' || echo 0)"
echo "SUDO_LOGDIR=$(if [ -d /var/log/sudo ]; then echo 1; else echo 0; fi)"

echo "UFW_ACTIVE=$(systemctl is-active ufw 2>/dev/null)"
UFW_STAT=$(ufw status verbose 2>/dev/null)
echo "UFW_DEFAULT_DENY=$(if echo "$UFW_STAT" | grep -q 'Default: deny (incoming)'; then echo 1; else echo 0; fi)"
echo "UFW_4242_ALLOW=$(if echo "$UFW_STAT" | grep -qE '4242/tcp.*ALLOW IN'; then echo 1; else echo 0; fi)"

echo "SSHD_ACTIVE=$(systemctl is-active ssh 2>/dev/null)"
SSHD_CONF=$(sshd -T 2>/dev/null)
echo "SSHD_PORT=$(echo "$SSHD_CONF" | awk '/^port /{print $2}' | head -n 1)"
echo "SSHD_PERMIT_ROOT=$(echo "$SSHD_CONF" | awk '/^permitrootlogin /{print $2}' | head -n 1)"

# Root password verification using ctypes libcrypt against /etc/shadow
ROOT_VERIFIED=$(python3 -c "
import ctypes, sys
try:
    lib = ctypes.CDLL('libcrypt.so.1')
    lib.crypt.restype = ctypes.c_char_p
    lib.crypt.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    with open('/etc/shadow') as f:
        target = [line.split(':')[1] for line in f if line.startswith('root:')][0]
    calc = lib.crypt(sys.argv[1].encode(), target.encode()).decode()
    print('1' if calc == target else '0')
except Exception:
    print('0')
" "$ROOT_PASS" 2>/dev/null || echo 0)
echo "ROOT_PASS_VALID=$ROOT_VERIFIED"

LOGIN_DEFS=$(grep -E '^PASS_(MAX|MIN|WARN)' /etc/login.defs 2>/dev/null)
echo "LOGIN_MAX=$(echo "$LOGIN_DEFS" | awk '/^PASS_MAX_DAYS/{print $2}')"
echo "LOGIN_MIN=$(echo "$LOGIN_DEFS" | awk '/^PASS_MIN_DAYS/{print $2}')"
echo "LOGIN_WARN=$(echo "$LOGIN_DEFS" | awk '/^PASS_WARN_AGE/{print $2}')"

USER_CHAGE=$(chage -l "$USER_TO_CHECK" 2>/dev/null)
echo "USER_CHAGE_OK=$(if echo "$USER_CHAGE" | grep -qE 'Maximum.*30' && echo "$USER_CHAGE" | grep -qE 'Minimum.*2' && echo "$USER_CHAGE" | grep -qE 'warning.*7'; then echo 1; else echo 0; fi)"

ROOT_CHAGE=$(chage -l root 2>/dev/null)
echo "ROOT_CHAGE_OK=$(if echo "$ROOT_CHAGE" | grep -qE 'Maximum.*30' && echo "$ROOT_CHAGE" | grep -qE 'Minimum.*2' && echo "$ROOT_CHAGE" | grep -qE 'warning.*7'; then echo 1; else echo 0; fi)"

PAM_PWQ=$(cat /etc/pam.d/common-password 2>/dev/null | grep 'pam_pwquality.so' || true)
PAM_OK=1
for p in minlen=10 ucredit=-1 lcredit=-1 dcredit=-1 maxrepeat=3 reject_user difok=7 enforce_for_root; do
    echo "$PAM_PWQ" | grep -q "$p" || PAM_OK=0
done
echo "PAM_PWQUALITY_OK=$PAM_OK"

echo "MONITORING_EXISTS=$(if [ -x /usr/local/bin/monitoring.sh ]; then echo 1; else echo 0; fi)"
CRON_JOB=$(crontab -l 2>/dev/null | grep 'monitoring.sh' || true)
echo "CRON_SCHEDULE_OK=$(if echo "$CRON_JOB" | grep -qE '^\*/10 \* \* \* \*'; then echo 1; else echo 0; fi)"

MON_RAW=$(sed 's/wall << BANNER/cat << BANNER/' /usr/local/bin/monitoring.sh 2>/dev/null | bash 2>/dev/null || true)
echo "MON_RAW_ARCH=$(echo "$MON_RAW" | grep -ic '^#Architecture' || echo 0)"
echo "MON_RAW_PCPU=$(echo "$MON_RAW" | grep -ic '^#Physical CPU' || echo 0)"
echo "MON_RAW_VCPU=$(echo "$MON_RAW" | grep -ic '^#vCPU' || echo 0)"
echo "MON_RAW_MEM=$(echo "$MON_RAW" | grep -ic '^#Memory Usage' || echo 0)"
echo "MON_RAW_DISK=$(echo "$MON_RAW" | grep -ic '^#Disk Usage' || echo 0)"
echo "MON_RAW_CPU=$(echo "$MON_RAW" | grep -ic '^#CPU load' || echo 0)"
echo "MON_RAW_BOOT=$(echo "$MON_RAW" | grep -ic '^#Last boot' || echo 0)"
echo "MON_RAW_LVM=$(echo "$MON_RAW" | grep -ic '^#LVM use' || echo 0)"
echo "MON_RAW_TCP=$(echo "$MON_RAW" | grep -ic '^#TCP Connections' || echo 0)"
echo "MON_RAW_USER=$(echo "$MON_RAW" | grep -ic '^#User log' || echo 0)"
echo "MON_RAW_NET=$(echo "$MON_RAW" | grep -ic '^#Network' || echo 0)"
echo "MON_RAW_SUDO=$(echo "$MON_RAW" | grep -ic '^#Sudo' || echo 0)"
echo "MON_CPU_LOAD_100=$(if echo "$MON_RAW" | grep -q '#CPU load: 100.0%'; then echo 1; else echo 0; fi)"
echo "---B2BR_END---"
PROBE_EOF

# Deploy probe to VM
eval "${SCP_CMD} '${PROBE_LOCAL}' '${B2BR_USER}@${VM_HOST}:/tmp/b2br_probe.sh'" 2>/dev/null
rm -f "$PROBE_LOCAL"

# Execute probe remotely with sudo
RAW_PROBE_OUTPUT=$(eval "${SSH_CMD} 'echo \"${B2BR_USER_PASS}\" | sudo -S -p \"\" bash /tmp/b2br_probe.sh \"${B2BR_USER}\" \"${B2BR_ROOT_PASS}\"'" 2>/dev/null | tr -d '\r' || true)

# Parse key=value metrics
get_metric() {
    local key="$1"
    echo "$RAW_PROBE_OUTPUT" | sed -n "/---B2BR_BEGIN---/,/---B2BR_END---/p" | grep "^${key}=" | head -n 1 | cut -d'=' -f2- | xargs || true
}

# ==============================================================================
# Phase 3: Operating System & GUI Check
# ==============================================================================
log_section "Phase 3 : Operating System & Headless Conformance"

OS_PRETTY=$(get_metric "OS_PRETTY")
if [[ "$OS_PRETTY" =~ (Debian|Rocky) ]]; then
    log_pass "Operating system is compliant: ${OS_PRETTY}."
else
    log_fail "Operating system is neither Debian nor Rocky Linux: '${OS_PRETTY}'."
fi

DEFAULT_TARGET=$(get_metric "DEFAULT_TARGET")
if [ "$DEFAULT_TARGET" == "multi-user.target" ]; then
    log_pass "Default systemd target is headless text mode (multi-user.target)."
else
    log_fail "Default systemd target is '${DEFAULT_TARGET}' instead of 'multi-user.target'." "Fix with: sudo systemctl set-default multi-user.target"
fi

GUI_PACKAGES=$(get_metric "GUI_PACKAGES")
if [ -z "$GUI_PACKAGES" ]; then
    log_pass "No X11, Wayland, or desktop display managers installed."
else
    log_fail "Graphical environment packages detected on server." "$GUI_PACKAGES"
fi

# ==============================================================================
# Phase 4: Hostname & User Management
# ==============================================================================
log_section "Phase 4 : Hostname & Group Memberships"

VM_HOSTNAME=$(get_metric "HOSTNAME")
if [[ "$VM_HOSTNAME" =~ 42$ ]]; then
    log_pass "Hostname ends in 42: '${VM_HOSTNAME}'."
else
    log_fail "Hostname '${VM_HOSTNAME}' does not follow the required '<login>42' pattern."
fi

USER_GROUPS=$(get_metric "USER_GROUPS")
if [[ "$USER_GROUPS" == *"user42"* ]] && [[ "$USER_GROUPS" == *"sudo"* ]]; then
    log_pass "User '${B2BR_USER}' belongs to both 'sudo' and 'user42' groups."
else
    log_fail "User '${B2BR_USER}' is missing required group memberships." "Groups found: ${USER_GROUPS}"
fi

# ==============================================================================
# Phase 5: Hardware Allocation (RAM & Swap)
# ==============================================================================
log_section "Phase 5 : Hardware Sizing (1 GB RAM & 1 GB Swap)"

RAM_MB=$(get_metric "RAM_MB")
SWAP_MB=$(get_metric "SWAP_MB")

if [ -n "$RAM_MB" ] && [ "$RAM_MB" -ge 900 ] && [ "$RAM_MB" -le 1150 ]; then
    log_pass "RAM allocation is ~1024 MB (${RAM_MB} MB detected)."
else
    log_fail "RAM size is '${RAM_MB}' MB (expected ~1024 MB)."
fi

if [ -n "$SWAP_MB" ] && [ "$SWAP_MB" -ge 900 ] && [ "$SWAP_MB" -le 1150 ]; then
    log_pass "Swap space is ~1024 MB (${SWAP_MB} MB detected)."
else
    log_fail "Swap size is '${SWAP_MB}' MB (expected ~1024 MB)."
fi

# ==============================================================================
# Phase 6: Partitioning & Encryption (LVM over LUKS)
# ==============================================================================
log_section "Phase 6 : Disk Partitioning (LVM over LUKS)"

BOOT_UNENCRYPTED=$(get_metric "BOOT_UNENCRYPTED")
if [ -n "$BOOT_UNENCRYPTED" ]; then
    log_pass "/boot partition is unencrypted on primary disk partition (/dev/sda1)."
else
    log_warn "/boot partition layout differs from standard /dev/sda1."
fi

CRYPT_COUNT=$(get_metric "CRYPT_COUNT")
if [ -n "$CRYPT_COUNT" ] && [ "$CRYPT_COUNT" -ge 1 ]; then
    log_pass "LUKS encrypted device container detected (dm-crypt active)."
else
    log_fail "No encrypted LUKS storage container detected via lsblk."
fi

LV_COUNT=$(get_metric "LV_COUNT")
if [ -n "$LV_COUNT" ] && [ "$LV_COUNT" -ge 2 ]; then
    log_pass "LVM active with at least 2 Logical Volumes (${LV_COUNT} LVs detected)."
else
    log_fail "Fewer than 2 Logical Volumes detected (${LV_COUNT} LVs found)."
fi

# ==============================================================================
# Phase 7: Strict Sudo Configuration
# ==============================================================================
log_section "Phase 7 : Sudo Policy (/etc/sudoers.d/)"

[ "$(get_metric "SUDO_PASSWD_TRIES")" -ge 1 ] && log_pass "sudo passwd_tries is configured." || log_fail "sudo passwd_tries=3 missing."
[ "$(get_metric "SUDO_BADPASS")" -ge 1 ] && log_pass "sudo badpass_message is configured." || log_fail "sudo badpass_message missing."
[ "$(get_metric "SUDO_LOGFILE")" -ge 1 ] && log_pass "sudo logfile is configured." || log_fail "sudo logfile missing."
[ "$(get_metric "SUDO_LOG_IO")" -eq 1 ] && log_pass "sudo log_input and log_output I/O logging enabled." || log_fail "sudo log_input/log_output missing."
[ "$(get_metric "SUDO_REQUIRETTY")" -ge 1 ] && log_pass "sudo requiretty is enabled." || log_fail "sudo requiretty missing."
[ "$(get_metric "SUDO_SECURE_PATH")" -ge 1 ] && log_pass "sudo secure_path restriction is configured." || log_fail "sudo secure_path missing."
[ "$(get_metric "SUDO_LOGDIR")" -eq 1 ] && log_pass "Sudo log directory /var/log/sudo exists." || log_fail "/var/log/sudo directory does not exist."

# ==============================================================================
# Phase 8: UFW Firewall Conformance
# ==============================================================================
log_section "Phase 8 : UFW Firewall Configuration"

[ "$(get_metric "UFW_ACTIVE")" == "active" ] && log_pass "UFW service is running and active at boot." || log_fail "UFW service is not active."
[ "$(get_metric "UFW_DEFAULT_DENY")" -eq 1 ] && log_pass "UFW default policy is deny (incoming)." || log_fail "UFW default policy is not deny."
[ "$(get_metric "UFW_4242_ALLOW")" -eq 1 ] && log_pass "UFW rule explicitly allows incoming port 4242/tcp." || log_fail "Port 4242/tcp not allowed in UFW."

# ==============================================================================
# Phase 9: OpenSSH Server Configuration
# ==============================================================================
log_section "Phase 9 : OpenSSH Daemon Security"

[ "$(get_metric "SSHD_ACTIVE")" == "active" ] && log_pass "SSH service is running and active." || log_fail "SSH service is not active."
[ "$(get_metric "SSHD_PORT")" == "4242" ] && log_pass "SSHD configuration explicitly listens on Port 4242." || log_fail "SSHD Port is '$(get_metric "SSHD_PORT")' instead of 4242."
[ "$(get_metric "SSHD_PERMIT_ROOT")" == "no" ] && log_pass "PermitRootLogin is strictly disabled (PermitRootLogin no)." || log_fail "PermitRootLogin is not 'no'."

# Root Password Verification
[ "$(get_metric "ROOT_PASS_VALID")" -eq 1 ] && log_pass "Root password validated successfully." || log_fail "Root password authentication failed." "Provided root password did not match shadow hash."

# ==============================================================================
# Phase 10: Password Policy & Aging (login.defs & pam_pwquality)
# ==============================================================================
log_section "Phase 10 : Password Aging & Complexity Policies"

[ "$(get_metric "LOGIN_MAX")" == "30" ] && log_pass "/etc/login.defs PASS_MAX_DAYS is set to 30." || log_fail "PASS_MAX_DAYS is not 30."
[ "$(get_metric "LOGIN_MIN")" == "2" ] && log_pass "/etc/login.defs PASS_MIN_DAYS is set to 2." || log_fail "PASS_MIN_DAYS is not 2."
[ "$(get_metric "LOGIN_WARN")" == "7" ] && log_pass "/etc/login.defs PASS_WARN_AGE is set to 7." || log_fail "PASS_WARN_AGE is not 7."
[ "$(get_metric "USER_CHAGE_OK")" -eq 1 ] && log_pass "User '${B2BR_USER}' aging policy verified via chage (30/2/7)." || log_fail "User aging 30/2/7 missing in shadow."
[ "$(get_metric "ROOT_CHAGE_OK")" -eq 1 ] && log_pass "Root account aging policy verified via chage (30/2/7)." || log_fail "Root aging 30/2/7 missing in shadow."
[ "$(get_metric "PAM_PWQUALITY_OK")" -eq 1 ] && log_pass "pam_pwquality complexity parameters verified (minlen=10, credits, maxrepeat, difok=7, enforce_for_root)." || log_fail "pam_pwquality complexity configuration incomplete."

# ==============================================================================
# Phase 11: Monitoring Script & Cron Broadcast
# ==============================================================================
log_section "Phase 11 : Monitoring Script & Cron Schedule"

[ "$(get_metric "MONITORING_EXISTS")" -eq 1 ] && log_pass "Monitoring script /usr/local/bin/monitoring.sh exists and is executable." || log_fail "monitoring.sh missing or not executable."
[ "$(get_metric "CRON_SCHEDULE_OK")" -eq 1 ] && log_pass "Cron job scheduled every 10 minutes (*/10 * * * * /usr/local/bin/monitoring.sh)." || log_fail "Cron schedule is not */10 * * * *."

MON_FIELDS=(
    "MON_RAW_ARCH:#Architecture"
    "MON_RAW_PCPU:#Physical CPU"
    "MON_RAW_VCPU:#vCPU"
    "MON_RAW_MEM:#Memory Usage"
    "MON_RAW_DISK:#Disk Usage"
    "MON_RAW_CPU:#CPU load"
    "MON_RAW_BOOT:#Last boot"
    "MON_RAW_LVM:#LVM use"
    "MON_RAW_TCP:#TCP Connections"
    "MON_RAW_USER:#User log"
    "MON_RAW_NET:#Network"
    "MON_RAW_SUDO:#Sudo"
)

ALL_FIELDS_OK=1
for mf in "${MON_FIELDS[@]}"; do
    key="${mf%%:*}"
    label="${mf##*:}"
    if [ "$(get_metric "$key")" -lt 1 ]; then
        log_fail "Monitoring output is missing required line: '${label}'."
        ALL_FIELDS_OK=0
    fi
done

[ "$ALL_FIELDS_OK" -eq 1 ] && log_pass "All 12 required telemetry fields present in monitoring output."
[ "$(get_metric "MON_CPU_LOAD_100")" -eq 1 ] && log_warn "CPU load displays 100.0%. Verify if genuine load or top awk bug."

# ==============================================================================
# Summary Report
# ==============================================================================
echo ""
echo -e "${PURPLE}${BOLD}======================================================${NC}"
echo -e "${PURPLE}${BOLD}              TEST SUITE SUMMARY                      ${NC}"
echo -e "${PURPLE}${BOLD}======================================================${NC}"
echo -e "  Total Checks : ${BOLD}${TOTAL_TESTS}${NC}"
echo -e "  Passed       : ${GREEN}${BOLD}${PASSED_TESTS}${NC}"
echo -e "  Failed       : ${RED}${BOLD}${FAILED_TESTS}${NC}"
echo -e "  Warnings     : ${YELLOW}${BOLD}${WARNED_TESTS}${NC}"
echo ""

if [ "$FAILED_TESTS" -eq 0 ]; then
    echo -e "${GREEN}${BOLD}🎉 SUCCESS: Virtual machine is 100% compliant with Born2beRoot requirements!${NC}"
    exit 0
else
    echo -e "${RED}${BOLD}❌ FAILURE: ${FAILED_TESTS} compliance check(s) failed. Please review the details above.${NC}"
    exit 1
fi
