#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

paddr_t page_translate(paddr_t addr); //implemented in pa4.2

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_n;
  if ((mmio_n = is_mmio(addr)) != -1) {
    return mmio_read(addr, len, mmio_n);
  }
  else {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int mmio_n;
	if ((mmio_n = is_mmio(addr)) != -1)
		mmio_write(addr, len, data, mmio_n);
	else
 		memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  //across the page boundary
  /*if(((addr & 0xfff) + len) > 0x1000){
    assert(0);
  }
  else{
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }*/
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  //across the page boundary
  /*if(((addr & 0xfff) + len) > 0x1000){
    assert(0);
  }
  else{
    paddr_t paddr = page_translate(addr);
    return paddr_write(paddr, data, len);
  }*/
  return paddr_write(addr, data, len);
}

paddr_t page_translate(paddr_t addr){
/*
  physical address: 0x xxxx xxxx xx    xx xxxx xxxx     xxxx xxxx xxxx
                                dir            page             offset
*/
  paddr_t dir = (addr >> 22) & 0x3ff;
  paddr_t page = (addr >> 12) & 0x3ff;
  paddr_t offset = addr & 0xfff;
  if(cpu.cr0.paging){
    uint32_t page_directory_base = cpu.cr3.page_directory_base;
    uint32_t page_table = paddr_read((page_directory_base << 12) + (dir << 2), 4);
    uint32_t page_frame = paddr_read((page_table & 0xffffff000) + (page << 2), 4);
    return  (page_frame & 0xffffff000) + offset;
  }
  return addr;
}
