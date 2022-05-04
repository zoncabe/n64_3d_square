
#include <assert.h>
#include <nusys.h>
#include "main.h"
#include "graphic.h"

static float theta;
static float triPos_x;
static float triPos_y;
static Vtx shade_vtx[] = {
    { -64,  64, -5, 0, 0, 0, 0, 0xff, 0, 0xff	},
    {  64,  64, -5, 0, 0, 0, 0, 0, 0, 0xff	},
    {  64, -64, -5, 0, 0, 0, 0, 0, 0xff, 0xff	},
    { -64, -64, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
};

void set_matrix(Dynamic *);
void draw_model(Dynamic* dynamicp);

void init_stage(void)
{
  triPos_x = 0.0;
  triPos_y = 0.0;
  theta = 0.0;
}

void set_matrix(Dynamic *dynamicp)
{
  guPerspective
  (
    &dynamicp->projection,
    &dynamicp->normal,
    50,
    (float)SCREEN_WD / (float)SCREEN_HT,
    10,
    10000, 
    1.0F 
  );

  guLookAt
  (
    &dynamicp->view,
    0, 300, 500,
    0, 0, 0,
    0, 1, 0
  );

  gSPPerspNormalize(glistp++, &dynamicp->normal);
  gSPLookAt(glistp++, &dynamicp->view);
  gSPMatrix(glistp++, &(dynamicp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
  gSPMatrix(glistp++, &(dynamicp->view), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
}

void make_dl(void)
{
  Dynamic* dynamicp;
  char conbuf[20]; 

  dynamicp = &gfx_dynamic[gfx_gtask_no];
  glistp = &gfx_glist[gfx_gtask_no][0];

  gfxRCPInit();
  gfxClearCfb();

  set_matrix (dynamicp);

  guRotate(&dynamicp->rotation, theta, 0.0F, 0.0F, 1.0F);
  guTranslate(&dynamicp->position, triPos_x, triPos_y, 0.0F);

  draw_model(dynamicp);

  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  assert((glistp - gfx_glist[gfx_gtask_no]) < GFX_GLIST_LEN);

  nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DEX , NU_SC_NOSWAPBUFFER);

  if(contPattern & 0x1)
    {
      /*
      nuDebConTextPos(0,12,23);
      sprintf(conbuf,"triPos_x=%5.1f",triPos_x);
      nuDebConCPuts(0, conbuf);

      nuDebConTextPos(0,12,24);
      sprintf(conbuf,"triPos_y=%5.1f",triPos_y);
      nuDebConCPuts(0, conbuf);
      */
    }
  else
    {
      nuDebConTextPos(0,9,24);
      nuDebConCPuts(0, "Controller1 not connect");
    }
    
  nuDebConDisp(NU_SC_SWAPBUFFER);

  gfx_gtask_no ^= 1;
}

void update(void)
{  
  static float vel = 1.0;

  nuContDataGetEx(contdata,0);

  triPos_x = contdata->stick_x;
  triPos_y = contdata->stick_y;

  if(contdata[0].trigger & A_BUTTON)
    {
      vel = -vel;
      osSyncPrintf("A button Push\n");
    }

  if(contdata[0].button & B_BUTTON)
    theta += vel * 3.0;
  else
    theta += vel;

  if(theta>360.0)
      theta-=360.0;
  else if (theta<0.0)
      theta+=360.0;
}

void draw_model(Dynamic* dynamicp)
{

  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->position)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->rotation)), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_NOPUSH);

  gSPVertex(glistp++,&(shade_vtx[0]),4, 0);

  gDPPipeSync(glistp++);
  gDPSetCycleType(glistp++,G_CYC_1CYCLE);
  gDPSetRenderMode(glistp++,G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
  gSPClearGeometryMode(glistp++,0xFFFFFFFF);
  gSPSetGeometryMode(glistp++,G_SHADE| G_SHADING_SMOOTH);
  //gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE )

  gSP2Triangles(glistp++,0,1,2,0,0,2,3,0);
}
