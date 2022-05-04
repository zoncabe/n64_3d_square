
#include <assert.h>
#include <nusys.h>
#include "graphic.h"

void SetViewMtx(Dynamic *);
void draw_square(Dynamic *dynamicp);

void SetViewMtx(Dynamic *dynamicp)
{
  u16 perspNorm;

  guPerspective
  (
    &dynamicp->projection,
    &perspNorm,
    50,
    (float)SCREEN_WD / (float)SCREEN_HT,
    10,
    10000,
    1.0F
  );

  guLookAt
  (
    &dynamicp->viewing,
    200, 200, 800,
    0, 0, 0,
    0, 1, 0
  );

  gSPPerspNormalize(glistp++, perspNorm);
  gSPLookAt(glistp++, &dynamicp->viewing);
  gSPMatrix(glistp++, &(dynamicp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
  gSPMatrix(glistp++, &(dynamicp->viewing), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
}

void makeDL00(void)
{
  glistp = gfx_glist;

  gfxRCPInit();
  gfxClearCfb();

  SetViewMtx(&gfx_dynamic);
  draw_square(&gfx_dynamic);

  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  assert(glistp - gfx_glist < GFX_GLIST_LEN);
  nuGfxTaskStart(gfx_glist, (s32)(glistp - gfx_glist) * sizeof (Gfx), NU_GFX_UCODE_F3DEX , NU_SC_SWAPBUFFER);
}

static Vtx square_vtx[] =  
{
  { -64,  64, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
  {  64,  64, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
  {  64, -64, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
  { -64, -64, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
};

void draw_square(Dynamic *dynamicp)
{

  guTranslate(&dynamicp->pos, 0, 0, 0);

  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->pos)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
  
  gSPVertex(glistp++,&(square_vtx[0]),4, 0);

  gDPPipeSync(glistp++);
  gDPSetCycleType(glistp++,G_CYC_1CYCLE);
  gDPSetRenderMode(glistp++,G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
  gSPClearGeometryMode(glistp++,0xFFFFFFFF);
  gSPSetGeometryMode(glistp++,G_SHADE| G_SHADING_SMOOTH);

  gSP2Triangles(glistp++,0,1,2,0,0,2,3,0);
}
