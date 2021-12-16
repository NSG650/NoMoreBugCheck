#include <ntddk.h>

CHAR KeBugCheckExOrignalBytes[14] = {0};
ULONG_PTR KeBugCheckExAddress;

NTSTATUS Overwrite(PVOID Address, PVOID Data, ULONG Size) {
	PHYSICAL_ADDRESS PhysAddress = MmGetPhysicalAddress(Address);
	PVOID MappedAddress = MmMapIoSpace(PhysAddress, Size, MmNonCached);

	if (MappedAddress == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	RtlCopyMemory(MappedAddress, Data, Size);
	MmUnmapIoSpace(MappedAddress, Size);
	return STATUS_SUCCESS;
}

VOID KeHookedBugCheckEx(ULONG BugCheckCode, ULONG_PTR Code1, ULONG_PTR Code2,
						ULONG_PTR Code3, ULONG_PTR Code4) {
	DbgPrint("[*] KeBugCheckEx was called by Process %d, thread id %d\n", PsGetCurrentProcessId(), PsGetCurrentThreadId());
	DbgPrint("[*] KeBugCheckEx(0x%llx, 0x%llx, 0x%llx, 0x%llx)\n", BugCheckCode,
			 Code1, Code2, Code3, Code4);
	LARGE_INTEGER Delay;

	Delay.LowPart = 0;
	Delay.HighPart = 0x80000000;

	KeDelayExecutionThread(KernelMode, FALSE, &Delay);
}


VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	NTSTATUS Status = Overwrite(KeBugCheckExAddress, (PVOID)KeBugCheckExOrignalBytes, 14);

	if (Status != STATUS_SUCCESS)
		DbgPrint("[!] Failed to restore the orignal KeBugCheckEx function\n");
	else
		DbgPrint("[+] Successfully restored the orignal KeBugCheckEx function\n");

	DbgPrint("[*] Goodbye Cruel World\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject,
					 PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;

	KeBugCheckExAddress = (ULONG_PTR)KeBugCheckEx;

	DbgPrint("[*] Hello World\n");
	DbgPrint("[*] KeBugCheckEx located at 0x%llx\n", KeBugCheckExAddress);
	DbgPrint("[*] KeHookedBugCheckEx located at 0x%llx\n", KeHookedBugCheckEx);
	RtlCopyMemory(KeBugCheckExOrignalBytes, KeBugCheckExAddress, 14);
	
	if (KeBugCheckExOrignalBytes[0])
		DbgPrint("[+] Copied over KeBugCheckEx\n");
	else {
		DbgPrint("[!] Failed to copy\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	for (INT i = 0; i < 14; i++)
		DbgPrint("[*] KeBugCheckExOrignalBytes[%d]: 0x%x\n", i,
				 KeBugCheckExOrignalBytes[i] & 0xff);

	#if defined(_M_X64)
	CHAR Patch[] = {
	    0x49, 0xba, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // mov r10, address
		0x41, 0xff, 0xe2 // jmp r10
	};

	ULONG_PTR KeHookedBugCheckExAddress = (ULONG_PTR)KeHookedBugCheckEx;
	CHAR *KeHookedBugCheckExAddressBytes = (CHAR*)&KeHookedBugCheckExAddress;

	RtlCopyMemory(&Patch[2], KeHookedBugCheckExAddressBytes, sizeof(ULONG_PTR));

	NTSTATUS Status = Overwrite(KeBugCheckExAddress, (PVOID)Patch, sizeof(Patch));
	
	if (Status != STATUS_SUCCESS) {
		DbgPrint("[!] Failed to overwrite KeBugCheckEx\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	DbgPrint("[+] Successfully overwrote KeBugCheckEx\n");
	#else
	DbgPrint("[!] Unknown architecture");
	return STATUS_FAILED_DRIVER_ENTRY;
	#endif

	CHAR Temp[14] = {0};
	RtlCopyMemory(Temp, KeBugCheckExAddress, 14);

	for (INT i = 0; i < 14; i++)
		DbgPrint("[*] KeBugCheckEx[%d]: 0x%x\n", i,
				 Temp[i] & 0xff);

	return STATUS_SUCCESS;
}