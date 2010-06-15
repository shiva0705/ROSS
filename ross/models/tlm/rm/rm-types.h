#ifndef INC_rm_types_h
#define INC_rm_types_h

struct rm_state_tag;
typedef struct rm_state_tag rm_state;
struct rm_particle_tag;
typedef struct rm_particle_tag rm_particle;
struct rm_message_tag;
typedef struct rm_message_tag rm_message;
struct rm_statistics_tag;
typedef struct rm_statistics_tag rm_statistics;
struct rm_pe_tag;
typedef struct rm_pe_tag rm_pe;

enum rm_event_t_tag;
typedef enum rm_event_t_tag rm_event_t;
enum rm_event_t_tag
{
	RM_PROXIMITY_LP = 100,
	RM_PROXIMITY_ENV = 101,

	// Now can add my own
	RM_SCATTER = 102,
	RM_PARTICLE = 103,
	RM_GATHER = 104,
	RM_WAVE_INIT = 105
};

struct rm_statistics_tag
{
	tw_stat	s_nparticles;
	tw_stat	s_ncell_scatter;
	tw_stat	s_ncell_gather;
	tw_stat	s_ncell_initiate;

};

struct rm_pe_tag
{
	FILE	*wave_log;
	FILE	*move_log;
};

struct rm_particle_tag
{
	double	 range;
	double	 freq;

	double	*position;
	double	*velocity;

	tw_lp	*user_lp;
};

struct rm_state_tag
{
	/*
	 * particles	-- queue of range particles
	 * nbrs		-- neighboring CELL LPs
	 */
	tw_memoryq	*particles;
	tw_lpid		*nbrs;

	/* next_time	-- next scatter time */
	tw_stime	 next_time;

	/*
	 * displacement		-- overall displacement
	 * displacements	-- displacement from a given direction
	 */
	double		 displacement;
	double		*displacements;

	/* per LP statistics */
	rm_statistics	*stats;
};

struct rm_message_tag
{
	rm_event_t	 type;
	tw_lpid		 id;
	double		 displacement;
	int		 direction;

	// RC-only variables
	double		 prev_time;
	double		 displ;
	double		 disp[6];
};

#endif
