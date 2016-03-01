#include <types.h>
#include <boot/realmode.h>
#include <fs/gpt.h>

#include <stdlib.h>

struct gpt_param gpt_param;
// pushad 함수의 레지스터 push 순서
// EAX(0x1C) -> ECX(0x18) -> EDX(0x14) -> EBX(0x10) -> ESP(0xC) -> EBP(0x8) -> ESI(0x4) -> EDI(0x0)
struct reg_param {
  union {
    struct {
      u32 edi;
      u32 esi;
      u32 ebp;
      u32 esp;
      u32 ebx;
      u32 edx;
      u32 ecx;
      u32 eax;

      /* -------- */
      /* Not used */
      u32 gsfs;
      u32 esds;
      /* -------- */

      u32 eflags;
    } r32;

    struct {
      u16 di, hdi;
      u16 si, hsi;
      u16 bp, hbp;
      u16 sp, hsp;
      u16 bx, hbx;
      u16 dx, hdx;
      u16 cx, hcx;
      u16 ax, hax;
      u16 fs, gs;
      u16 ds, es;
      u16 flags, hflags;
    } __attribute__((packed)) r16;

    struct {
      u8 dil, dih, hdil, hdih;
      u8 sil, sih, hsil, hsih;
      u8 bpl, bph, hbpl, hbph;
      u8 spl, sph, hspl, hsph;
      u8 bl,  bh,  hbl,  hbh;
      u8 dl,  dh,  hdl,  hdh;
      u8 cl,  ch,  hcl,  hch;
      u8 al,  ah,  hal,  hah;

      /* -------- */
      /* Not used */
      u8 fsl, fsh, gsl,  gsh;
      u8 dsl, dsh, esl,  esh;
      u8 flagsl, flagsh, hflagsl, hflagsh;
      /* -------- */
    } __attribute__((packed)) r8;
  };
};

inline void get_registers(struct reg_param& regs) {
  // __asm__ __volatile__ (asms : output : input : clobber);
  struct reg_param* reg_param_origin = nullptr;

  asm volatile (
    "pushad               \n\t"
    "movd %%esp, %0       \n\t"
    :
    : "m" (reg_param_origin)
  );

  memcpy(&regs, reg_param_origin, sizeof(struct reg_param));
  asm volatile ( "popad" :: );
}

struct u32 intcall(const u8 callno, struct reg_param* regs) {
  u32 result_eax = 0;
  struct reg_param* reg_param_origin = nullptr;

  asm volatile (
    "pushad               \n\t"
    "movd %%esp, %0       \n\t"
    :
    : "m" (reg_param_origin)
  );

  memcpy(reg_param_origin, regs, sizeof(struct reg_param));
  asm volatile (
    "popad                \n\t"
    "int %0               \n\t"
    : "=&a" (result_eax)
    : "i" (callno)
  );

  return result_eax;
}

void main() {
  for (int i = 0; i < 4; ++i) {
    struct gpt_entry& entry = gpt_param.gpts[i];
    if ((entry.bootIndicator & 1) == 0) {
      u32 sector = entry.type.chs.relativeSector;
      u16 seclen = 512;
    }
  }
}

