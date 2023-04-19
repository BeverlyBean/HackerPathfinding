
void bhv_PathNodeMaestro_init(void) {
	o->oMaestroInit = 0;
}
void bhv_PathNodeMaestro_loop(void) {
	if (o->oMaestroInit == 0) {
		// gpf_setup_neighbors();
		o->oMaestroInit = 1;
	}
}
