
## UEFI Driver Compilation Instructions

UEFI is WIP

This driver is currently able to start the BF hypervisor on one core.  export EDK_PATH= to the absolute path of your EDK tree and make.

Two findings need to be accounted for in the continuation of this work:

1) TR needs to be pointing to a valid TSS descriptor/TSS while starting hypervisor.  The TR appears to reset each time BSP is switched to another core, though TSS descriptor/TSS remains.  PrepareProcessor() sets up TR but also allocates another GDT, so TR setting should be broken out.  After the hypervisor is started on the core, perhaps the old GDT should be reloaded and the new one deallocated for cleanliness.

2) The SwitchBSP function swaps GDT, IDT, stack, CR0, and CR4 with the core the BSP is switching to.  Since cr4.VMXE=0 on the core being switched to, this causes the firmware to attempt to set VMXE=0 on the core the hypervisor is running on.  Either make an exit handler for the hypervisor to prevent this, or manually set VMXE=1 on all cores ahead of time (though hypervisor also throws an exception on this but Rian said that check should probably be relaxed)



