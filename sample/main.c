/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/gxm.h>

#include <vita2d_fbo.h>
#include "includes/lcd3x_v.h"
#include "includes/lcd3x_f.h"
#include "includes/gtu_v.h"
#include "includes/gtu_f.h"
#include "includes/texture_v.h"
#include "includes/texture_f.h"
#include "includes/opaque_v.h"

#include "includes/bicubic_f.h"
#include "includes/xbr_2x_v.h"
#include "includes/xbr_2x_f.h"
#include "includes/xbr_2x_fast_v.h"
#include "includes/xbr_2x_fast_f.h"
#include "includes/advanced_aa_v.h"
#include "includes/advanced_aa_f.h"
#include "includes/scale2x_f.h"
#include "includes/scale2x_v.h"
#include "includes/sharp_bilinear_f.h"
#include "includes/sharp_bilinear_v.h"
#include "includes/xbr_2x_noblend_f.h"
#include "includes/xbr_2x_noblend_v.h"
#include "includes/fxaa_v.h"
#include "includes/fxaa_f.h"
#include "includes/crt_easymode_f.h"

/*
 * Symbol of the image.png file
 */
extern unsigned char _binary_psp_png_start;
extern const SceGxmProgram texture_v_gxp_start;


int main()
{
	SceCtrlData pad;
	vita2d_texture *image;
	vita2d_texture *fbo;
	//vita2d_texture *fbo2;


	float rad = 0.0f;
	int selected = 0;

	vita2d_init();

	vita2d_shader* lcd3x_shader = vita2d_create_shader((SceGxmProgram*) lcd3x_v, (SceGxmProgram*) lcd3x_f);
	vita2d_shader* bicubic_shader = vita2d_create_shader((SceGxmProgram*) opaque_v, (SceGxmProgram*) texture_f);
	//vita2d_shader* xbr = vita2d_create_shader((SceGxmProgram*) xbr_2x_v, (SceGxmProgram*) xbr_2x_f);
	//vita2d_shader* xbr_fast = vita2d_create_shader((SceGxmProgram*) xbr_2x_fast_v, (SceGxmProgram*) xbr_2x_fast_f);
	vita2d_shader* gtu = vita2d_create_shader((SceGxmProgram*) scale2x_v, (SceGxmProgram*) scale2x_f);
	vita2d_shader* sharp = vita2d_create_shader((SceGxmProgram*) opaque_v, (SceGxmProgram*) crt_easymode_f);



	
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));


	/*
	 * Load the statically compiled image.png file.
	 */
	image = vita2d_load_PNG_buffer(&_binary_psp_png_start);
  fbo = vita2d_create_empty_texture(960,544);
	//fbo2 = vita2d_create_empty_texture(960,544);

	memset(&pad, 0, sizeof(pad));
	
	

	while (1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);

		if (pad.buttons & SCE_CTRL_START){
			vita2d_texture_set_filters(image, SCE_GXM_TEXTURE_FILTER_POINT,SCE_GXM_TEXTURE_FILTER_POINT);
		} else if(pad.buttons & SCE_CTRL_SQUARE){
			vita2d_texture_set_filters(image, SCE_GXM_TEXTURE_FILTER_LINEAR,SCE_GXM_TEXTURE_FILTER_LINEAR);
		} else if(pad.buttons & SCE_CTRL_CIRCLE){
			selected = 2;

		} else if(pad.buttons & SCE_CTRL_TRIANGLE){
			selected = 0;
		}else if(pad.buttons & SCE_CTRL_CROSS){
			selected = 1;
		} else if(pad.buttons & SCE_CTRL_SELECT){
			abort();
			break;
		}
		vita2d_shader* shader = NULL;
		
		switch (selected) {
			case 0:
				shader = bicubic_shader;
				break;
			case 1:
				shader = lcd3x_shader;
				break;
			case 2:
				shader = sharp;
				break;
		}

		vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
		vita2d_texture_set_program(shader->vertexProgram, shader->fragmentProgram);
		vita2d_texture_set_wvp(shader->wvpParam);
		vita2d_texture_set_vertexInput(&shader->vertexInput);
		vita2d_texture_set_fragmentInput(&shader->fragmentInput);
		vita2d_draw_texture_scale(image, 0, 0, 2, 2);

		vita2d_end_drawing();
		
		/*vita2d_start_drawing_advanced(fbo2, VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY | VITA_2D_SCENE_VERTEX_WAIT_FOR_DEPENDENCY);
		

		vita2d_texture_set_program(bicubic_shader->vertexProgram, bicubic_shader->fragmentProgram);
		vita2d_texture_set_wvp(bicubic_shader->wvpParam);
		vita2d_texture_set_texSize(bicubic_shader->texSizeParam);
		vita2d_texture_set_texSizeF(NULL);

		vita2d_draw_texture_scale(fbo, 0, 0, 1.0f, 1.0f);

		vita2d_end_drawing();*/
		
		
		vita2d_start_drawing_advanced(NULL, VITA_2D_SCENE_VERTEX_WAIT_FOR_DEPENDENCY);
		
		vita2d_texture_set_program(bicubic_shader->vertexProgram, bicubic_shader->fragmentProgram);
		vita2d_texture_set_wvp(bicubic_shader->wvpParam);
		vita2d_texture_set_vertexInput(&bicubic_shader->vertexInput);
		vita2d_texture_set_fragmentInput(&bicubic_shader->fragmentInput);
		vita2d_draw_texture(fbo,0,0);
		
    /*vita2d_start_drawing();
		vita2d_draw_texture(image,0,0);*/
		vita2d_end_drawing();
		vita2d_swap_buffers();
		


		rad += 0.1f;
	}

	/*
	 * vita2d_fini() waits until the GPU has finished rendering,
	 * then we can free the assets freely.
	 */
	/*vita2d_wait_rendering_done();
	vita2d_free_shader(bicubic_shader);
	vita2d_free_shader(lcd3x_shader);*/
	vita2d_fini();

	vita2d_free_texture(image);

	sceKernelExitProcess(0);
	return 0;
}
