set -euo pipefail

IMG=ext2.img
SIZE=128M
MNT=ext2mnt
GET=./getinode
BS=1024

cleanup() {
    [[ -n ${LOOP:-} && -b ${LOOP:-/dev/null} ]] && {
        sudo umount "$MNT" 2>/dev/null || true
        sudo losetup -d "$LOOP" 2>/dev/null || true
    }
    rm -rf "$MNT" "$IMG"
}
trap cleanup EXIT INT TERM

echo "1) creating image ($SIZE)"
truncate --size "$SIZE" "$IMG"

echo "2) format ext2 (bs=$BS)"
mkfs.ext2 -q -b "$BS" "$IMG"

echo "3) include loop and mount"
mkdir -p "$MNT"
LOOP=$(sudo losetup -f --show "$IMG")
sudo mount -t ext2 "$LOOP" "$MNT"

echo "4) files for testing"
echo "Hello World" | sudo tee "$MNT/hello.txt" >/dev/null
sudo dd if=/dev/urandom of="$MNT/big.bin" bs=1M count=70 status=none
sudo dd if=/dev/zero of="$MNT/sparse.bin" seek=150 count=0 bs=1M status=none

echo "5) get inode and calc sha"
declare -A SHA INODE
for f in hello.txt big.bin sparse.bin; do
    INODE[$f]=$(stat -c '%i' "$MNT/$f")
    SHA[$f]=$(sha512sum "$MNT/$f" | awk '{print $1}')
    printf "  %-10s inode=%-6s sha=%s\n" "$f" "${INODE[$f]}" "${SHA[$f]}"
done

echo "6) unmount"
sudo umount "$MNT"
sudo losetup -d "$LOOP"
unset LOOP
echo "done."

echo "7) checking sha"
RET=0
for f in "${!INODE[@]}"; do
    got=$( "$GET" "$IMG" "${INODE[$f]}" | sha512sum | awk '{print $1}')
    if [[ "$got" == "${SHA[$f]}" ]]; then
        printf "  %-10s : \e[32mOK\e[0m\n" "$f"
    else
        printf "  %-10s : \e[31mFAIL\e[0m  exp=%s got=%s\n" "$f" "${SHA[$f]}" "$got"
        RET=1
    fi
done

exit $RET
