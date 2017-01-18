#ifndef FRONTEND_PARAMETER
#define FRONTEND_PARAMETER


#if defined(DVB_API_VERSION) && DVB_API_VERSION>=5

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int		uint32_t;
# define __uint32_t_defined
#endif

struct frontend_parameters {
    fe_delivery_system_t    delivery_system;
    uint32_t                frequency;
    fe_spectral_inversion_t inversion;

    union {
        struct dvb_qpsk_parameters qpsk;
        struct dvb_qam_parameters  qam;
        struct dvb_ofdm_parameters ofdm;
        struct dvb_vsb_parameters vsb;
        struct isdbt_parameters
        {
            uint32_t            bandwidth_hz;
            fe_transmit_mode_t  transmission_mode;
            fe_guard_interval_t guard_interval;
            uint8_t             isdbt_partial_reception;
            uint8_t             isdbt_sb_mode;
            uint8_t             isdbt_sb_subchannel;
            uint32_t            isdbt_sb_segment_idx;
            uint32_t            isdbt_sb_segment_count;
            uint8_t             isdbt_layer_enabled;
            struct {
                uint8_t         segment_count;
                fe_code_rate_t  fec;
                fe_modulation_t modulation;
                uint8_t         interleaving;
            } layer[3];
        } isdbt;
    } u;
};
#endif

#endif
