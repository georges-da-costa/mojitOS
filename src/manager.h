void init_manager(const char** labels, int nb_sensors, int stat_mode);
void use_manager(struct timespec ts,
		 const uint64_t* values, int nb_sensors,
		 uint64_t stat_data);
