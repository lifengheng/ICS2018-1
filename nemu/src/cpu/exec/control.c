#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
	// the target address is calculated at the decode stage
	rtl_j(decoding.jmp_eip);

	print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
	// the target address is calculated at the decode stage
	uint32_t cc = decoding.opcode & 0xf;
	rtl_setcc(&t0, cc);
	rtl_li(&t1, 0);
	rtl_jrelop(RELOP_NE, &t0, &t1, decoding.jmp_eip);

	print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
	rtl_jr(&id_dest->val);

	print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
	// the target address is calculated at the decode stage
	// TODO();

	t0 = decoding.seq_eip;
	// printf("call 0x%x\n", t0);
	rtl_push(&t0);
	rtl_j(decoding.jmp_eip);
	print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
	// TODO();
	rtl_pop(&decoding.jmp_eip);
	// printf("ret 0x%x\n", decoding.jmp_eip);
	rtl_j(decoding.jmp_eip);

	print_asm("ret");
}

make_EHelper(call_rm) {
	// TODO();
	t0 = decoding.seq_eip;
	// printf("call_rm 0x%x\n", t0);
	rtl_push(&t0);
	rtl_jr(&id_dest->val);
	print_asm("call *%s", id_dest->str);
}

make_EHelper(stc){
  rtl_li(&t0, 1);
  rtl_set_CF(&t0);

  print_asm("stc");
}
