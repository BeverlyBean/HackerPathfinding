Lights1 dbgArrow_f3dlite_material_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFF, 0xFF, 0xFF, 0x28, 0x28, 0x28);

Vtx dbgArrow_Cube_mesh_layer_1_vtx_cull[8] = {
	{{ {-101, -100, 306}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {-101, 100, 306}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {-101, 100, -21}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {-101, -100, -21}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {99, -100, 306}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {99, 100, 306}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {99, 100, -21}, 0, {-16, -16}, {0, 0, 0, 0} }},
	{{ {99, -100, -21}, 0, {-16, -16}, {0, 0, 0, 0} }},
};

Vtx dbgArrow_Cube_mesh_layer_1_vtx_0[18] = {
	{{ {-1, -100, 306}, 0, {112, 496}, {0, 129, 0, 255} }},
	{{ {-101, -100, -21}, 0, {368, 496}, {0, 129, 0, 255} }},
	{{ {99, -100, -21}, 0, {368, 240}, {0, 129, 0, 255} }},
	{{ {-101, 100, -21}, 0, {624, 496}, {0, 127, 0, 255} }},
	{{ {-1, 100, 306}, 0, {880, 240}, {0, 127, 0, 255} }},
	{{ {99, 100, -21}, 0, {624, 240}, {0, 127, 0, 255} }},
	{{ {-1, -100, 306}, 0, {368, 752}, {134, 0, 37, 255} }},
	{{ {-1, 100, 306}, 0, {624, 752}, {134, 0, 37, 255} }},
	{{ {-101, 100, -21}, 0, {624, 496}, {134, 0, 37, 255} }},
	{{ {-101, -100, -21}, 0, {368, 496}, {134, 0, 37, 255} }},
	{{ {-101, -100, -21}, 0, {368, 496}, {0, 0, 129, 255} }},
	{{ {-101, 100, -21}, 0, {624, 496}, {0, 0, 129, 255} }},
	{{ {99, 100, -21}, 0, {624, 240}, {0, 0, 129, 255} }},
	{{ {99, -100, -21}, 0, {368, 240}, {0, 0, 129, 255} }},
	{{ {99, -100, -21}, 0, {368, 240}, {122, 0, 37, 255} }},
	{{ {99, 100, -21}, 0, {624, 240}, {122, 0, 37, 255} }},
	{{ {-1, 100, 306}, 0, {624, -16}, {122, 0, 37, 255} }},
	{{ {-1, -100, 306}, 0, {368, -16}, {122, 0, 37, 255} }},
};

Gfx dbgArrow_Cube_mesh_layer_1_tri_0[] = {
	gsSPVertex(dbgArrow_Cube_mesh_layer_1_vtx_0 + 0, 18, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
	gsSP2Triangles(6, 7, 8, 0, 6, 8, 9, 0),
	gsSP2Triangles(10, 11, 12, 0, 10, 12, 13, 0),
	gsSP2Triangles(14, 15, 16, 0, 14, 16, 17, 0),
	gsSPEndDisplayList(),
};


Gfx mat_dbgArrow_f3dlite_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(dbgArrow_f3dlite_material_lights),
	gsSPEndDisplayList(),
};

Gfx dbgArrow_Cube_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(dbgArrow_Cube_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_dbgArrow_f3dlite_material),
	gsSPDisplayList(dbgArrow_Cube_mesh_layer_1_tri_0),
	gsSPEndDisplayList(),
};

Gfx dbgArrow_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

