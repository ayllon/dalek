.phony: kernel boot iso

kernel:
	cargo xbuild --target=x86_64-bare.json --verbose

dalek.iso: kernel iso/grub.cfg
	mkdir -p "target/isofiles/boot/grub"
	cp "target/x86_64-bare/debug/rusty_dalek" "target/isofiles/boot/rusty_dalek"
	cp "iso/grub.cfg" "target/isofiles/boot/grub"
	grub2-mkrescue -o "dalek.iso" "target/isofiles"

iso: dalek.iso

boot: dalek.iso
	qemu-system-x86_64 -cdrom "dalek.iso"

