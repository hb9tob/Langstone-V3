#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Lang Trx Pluto
# GNU Radio version: 3.10.5.1

from gnuradio import analog
from gnuradio import audio
from gnuradio import blocks
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio import gr
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import iio
from gnuradio import network
from gnuradio.fft import logpwrfft
from gnuradio import anr
import os
import errno
import math



class Lang_TRX_Pluto(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Lang Trx Pluto", catch_exceptions=True)

        ##################################################
        # Variables
        ##################################################
        plutoip=os.environ.get('PLUTO_IP')
        if plutoip==None :
          plutoip='pluto.local'
        plutoip='ip:' + plutoip
        self.Tx_Mode = Tx_Mode = 0
        self.Tx_LO = Tx_LO = 1000000000
        self.Tx_Gain = Tx_Gain = 0
        self.Tx_Filt_Low = Tx_Filt_Low = 300
        self.Tx_Filt_High = Tx_Filt_High = 3000
        self.ToneBurst = ToneBurst = False
        self.Rx_Mute = Rx_Mute = False
        self.Rx_Mode = Rx_Mode = 0
        self.Rx_LO = Rx_LO = 1000000000
        self.Rx_Gain = Rx_Gain = 30
        self.Rx_Filt_Low = Rx_Filt_Low = 300
        self.Rx_Filt_High = Rx_Filt_High = 3000
        self.RxOffset = RxOffset = 0
        self.PTT = PTT = False
        self.MicGain = MicGain = 5.0
        self.KEY = KEY = False
        self.FMMIC = FMMIC = 50
        self.FFT_SEL = FFT_SEL = 1
        self.CTCSS = CTCSS = 885
        self.AMMIC = AMMIC = 5
        self.AFGain = AFGain = 100

        # COMP TX compressor parameters
        self.comp_agc_attack  = 0.041
        self.comp_agc_decay   = 0.033
        self.comp_agc_ref     = 0.90
        self.comp_agc_max     = 100.0
        self.comp_lpf_cutoff  = 2700
        self.comp_eq1_freq    = 500;   self.comp_eq1_gain = 0.0
        self.comp_eq2_freq    = 1200;  self.comp_eq2_gain = 4.0
        self.comp_eq3_freq    = 2500;  self.comp_eq3_gain = 6.0
        self.comp_mic_gain    = 50     # 0-100, maps to gain 0.0-1.0 when COMP active

        # NB1 spectral noise reduction parameters
        self.nb1_algorithm  = 0
        self.nb1_fft_size   = 256
        self.nb1_overlap    = 4
        self.nb1_alpha      = 0.98
        self.nb1_beta       = 2.0
        self.nb1_gain_floor = 0.01

        ##################################################
        # Blocks
        ##################################################

        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=11,
                decimation=1,
                taps=[],
                fractional_bw=0)
        self.network_udp_sink_1 = network.udp_sink(gr.sizeof_float, 1, '127.0.0.1', 7474, 0, 2048, False)
        self.network_udp_sink_0 = network.udp_sink(gr.sizeof_float, 1, '127.0.0.1', 7373, 0, 2048, False)
        self.low_pass_filter_0 = filter.fir_filter_fff(
            1,
            firdes.low_pass(
                1,
                48000,
                3000,
                1000,
                window.WIN_HAMMING,
                6.76))
        self.logpwrfft_x_0_0 = logpwrfft.logpwrfft_c(
            sample_rate=(48000/ (2** FFT_SEL)),
            fft_size=512,
            ref_scale=2,
            frame_rate=15,
            avg_alpha=0.9,
            average=True,
            shift=False)
        self.logpwrfft_x_0 = logpwrfft.logpwrfft_c(
            sample_rate=(48000/ (2 ** FFT_SEL)),
            fft_size=512,
            ref_scale=2,
            frame_rate=15,
            avg_alpha=0.9,
            average=True,
            shift=False)
        self.iio_pluto_source_0 = iio.fmcomms2_source_fc32(plutoip, [True, True], 0x800)
        self.iio_pluto_source_0.set_len_tag_key('packet_len')
        self.iio_pluto_source_0.set_frequency(Rx_LO)
        self.iio_pluto_source_0.set_samplerate(528000)
        self.iio_pluto_source_0.set_gain_mode(0, 'slow_attack')
        self.iio_pluto_source_0.set_gain(0, 64)
        self.iio_pluto_source_0.set_quadrature(True)
        self.iio_pluto_source_0.set_rfdc(True)
        self.iio_pluto_source_0.set_bbdc(True)
        self.iio_pluto_source_0.set_filter_params('Auto', '', 0, 0)
        self.iio_pluto_sink_0 = iio.fmcomms2_sink_fc32(plutoip, [True, True], 0x800, False)
        self.iio_pluto_sink_0.set_len_tag_key('')
        self.iio_pluto_sink_0.set_bandwidth(2000000)
        self.iio_pluto_sink_0.set_frequency(Tx_LO)
        self.iio_pluto_sink_0.set_samplerate(528000)
        self.iio_pluto_sink_0.set_attenuation(0, Tx_Gain)
        self.iio_pluto_sink_0.set_filter_params('Auto', '', 0, 0)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(11, firdes.low_pass(1,529200,23000,2000), RxOffset, 528000)
        self.blocks_vector_to_stream_0_0 = blocks.vector_to_stream(gr.sizeof_float*1, 512)
        self.blocks_vector_to_stream_0 = blocks.vector_to_stream(gr.sizeof_float*1, 512)
        self.blocks_mute_xx_0_0_0 = blocks.mute_cc(bool((not PTT) or (Tx_Mode==2 and not KEY) or (Tx_Mode==3 and not KEY)))
        self.blocks_multiply_const_vxx_4 = blocks.multiply_const_cc((Tx_Mode < 4) or (Tx_Mode==5))
        self.blocks_multiply_const_vxx_3 = blocks.multiply_const_cc(Tx_Mode==4)
        self.blocks_multiply_const_vxx_2_1_0 = blocks.multiply_const_ff((1.0 + (Rx_Mode==5)))
        self.blocks_multiply_const_vxx_2_1 = blocks.multiply_const_ff(Rx_Mode==5)
        self.blocks_multiply_const_vxx_2_0 = blocks.multiply_const_ff(((Rx_Mode==4) * 0.2))
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_ff(Rx_Mode<4)
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_ff(((AFGain/100.0) *  (not Rx_Mute)))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_ff((FMMIC/5.0))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_ff(((MicGain)*(int(Tx_Mode==0)) + (MicGain)*(int(Tx_Mode==1)) + (AMMIC/10.0)*(int(Tx_Mode==5)) ))
        self.blocks_keep_one_in_n_0_0 = blocks.keep_one_in_n(gr.sizeof_gr_complex*1, (2 ** FFT_SEL))
        self.blocks_keep_one_in_n_0 = blocks.keep_one_in_n(gr.sizeof_gr_complex*1, (2 ** FFT_SEL))
        _fft_aa_cutoff = 48000 / (2 ** (FFT_SEL + 1)) * 0.9 if FFT_SEL > 0 else 22000
        _fft_aa_trans = max(200, 48000 / (2 ** (FFT_SEL + 1)) * 0.1) if FFT_SEL > 0 else 2000
        self.fir_filter_xxx_fft_aa = filter.fir_filter_ccc(
            1,
            firdes.low_pass(1, 48000, _fft_aa_cutoff, _fft_aa_trans, window.WIN_HAMMING, 6.76))
        self.blocks_float_to_complex_0_0 = blocks.float_to_complex(1)
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_complex_to_real_0_0 = blocks.complex_to_real(1)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_1_0 = blocks.add_vff(1)
        self.blocks_add_xx_1 = blocks.add_vff(1)
        self.blocks_add_xx_0_0 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.blocks_add_const_vxx_0_0 = blocks.add_const_ff(((0.5 * int(Tx_Mode==5)) + int(Tx_Mode==2) +int(Tx_Mode==3)))
        self.band_pass_filter_1 = filter.fir_filter_fff(
            1,
            firdes.band_pass(
                1,
                48000,
                300,
                3500,
                100,
                window.WIN_HAMMING,
                6.76))
        self.band_pass_filter_0_0 = filter.fir_filter_ccc(
            1,
            firdes.complex_band_pass(
                1,
                48000,
                Tx_Filt_Low,
                Tx_Filt_High,
                100,
                window.WIN_HAMMING,
                6.76))
        self.band_pass_filter_0 = filter.fir_filter_ccc(
            1,
            firdes.complex_band_pass(
                1,
                48000,
                Rx_Filt_Low,
                Rx_Filt_High,
                100,
                window.WIN_HAMMING,
                6.76))
        self.audio_source_0 = audio.source(48000, "hw:CARD=S3,DEV=0", True)
        self.audio_sink_0 = audio.sink(48000, "hw:CARD=S3,DEV=0", False)
        self.analog_sig_source_x_1_0 = analog.sig_source_f(48000, analog.GR_SIN_WAVE, (CTCSS/10.0), (0.15 * (CTCSS >0)), 0, 0)
        self.analog_sig_source_x_1 = analog.sig_source_f(48000, analog.GR_COS_WAVE, 1750, (1.0*ToneBurst), 0, 0)

        self.analog_rail_ff_0_0 = analog.rail_ff((-0.99), 0.99)
        self.analog_rail_ff_0 = analog.rail_ff((-1), 1)
        self.analog_nbfm_tx_0 = analog.nbfm_tx(
        	audio_rate=48000,
        	quad_rate=48000,
        	tau=(75e-6),
        	max_dev=5000,
        	fh=(-1),
                )
        self.analog_nbfm_rx_0 = analog.nbfm_rx(
        	audio_rate=48000,
        	quad_rate=48000,
        	tau=(75e-6),
        	max_dev=5e3,
          )
        self.analog_const_source_x_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)
        self.analog_agc3_xx_0 = analog.agc3_cc((1e-2), (5e-7), 0.1, 1.0, 1)
        self.analog_agc3_xx_0.set_max_gain(1000)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_agc3_xx_0, 0), (self.blocks_complex_to_real_0_0, 0))
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_float_to_complex_0_0, 1))
        self.connect((self.analog_nbfm_rx_0, 0), (self.blocks_multiply_const_vxx_2_0, 0))
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_const_vxx_3, 0))
        self.connect((self.analog_rail_ff_0, 0), (self.band_pass_filter_1, 0))
        self.connect((self.analog_rail_ff_0_0, 0), (self.blocks_float_to_complex_0_0, 0))
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_add_xx_0_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.analog_nbfm_rx_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_real_0, 0))
        self.connect((self.band_pass_filter_0_0, 0), (self.blocks_multiply_const_vxx_4, 0))
        self.connect((self.band_pass_filter_1, 0), (self.blocks_add_xx_0_0, 1))
        self.connect((self.blocks_add_const_vxx_0_0, 0), (self.analog_rail_ff_0_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.analog_rail_ff_0, 0))
        self.connect((self.blocks_add_xx_0_0, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.blocks_add_xx_1, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.blocks_add_xx_1_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.blocks_mute_xx_0_0_0, 0))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.blocks_multiply_const_vxx_2_1, 0))
        self.connect((self.blocks_complex_to_real_0, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.blocks_complex_to_real_0_0, 0), (self.blocks_multiply_const_vxx_2_1_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.analog_agc3_xx_0, 0))
        self.connect((self.blocks_float_to_complex_0_0, 0), (self.band_pass_filter_0_0, 0))
        self.connect((self.blocks_keep_one_in_n_0, 0), (self.logpwrfft_x_0, 0))
        self.connect((self.blocks_keep_one_in_n_0_0, 0), (self.logpwrfft_x_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_const_vxx_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.low_pass_filter_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.blocks_add_xx_1_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2_0, 0), (self.blocks_add_xx_1, 1))
        self.connect((self.blocks_multiply_const_vxx_2_1, 0), (self.blocks_add_xx_1_0, 1))
        self.connect((self.blocks_multiply_const_vxx_2_1_0, 0), (self.blocks_add_xx_1, 0))
        self.connect((self.blocks_multiply_const_vxx_3, 0), (self.blocks_add_xx_2, 0))
        self.connect((self.blocks_multiply_const_vxx_4, 0), (self.blocks_add_xx_2, 1))
        self.connect((self.blocks_mute_xx_0_0_0, 0), (self.blocks_keep_one_in_n_0_0, 0))
        self.connect((self.blocks_mute_xx_0_0_0, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.blocks_vector_to_stream_0, 0), (self.network_udp_sink_0, 0))
        self.connect((self.blocks_vector_to_stream_0_0, 0), (self.network_udp_sink_1, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.band_pass_filter_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.fir_filter_xxx_fft_aa, 0))
        self.connect((self.fir_filter_xxx_fft_aa, 0), (self.blocks_keep_one_in_n_0, 0))
        self.connect((self.iio_pluto_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.logpwrfft_x_0, 0), (self.blocks_vector_to_stream_0, 0))
        self.connect((self.logpwrfft_x_0_0, 0), (self.blocks_vector_to_stream_0_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.audio_sink_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.iio_pluto_sink_0, 0))


    def get_Tx_Mode(self):
        return self.Tx_Mode

    def set_Tx_Mode(self, Tx_Mode):
        self.Tx_Mode = Tx_Mode
        self.blocks_add_const_vxx_0_0.set_k(((0.5 * int(self.Tx_Mode==5)) + int(self.Tx_Mode==2) +int(self.Tx_Mode==3)))
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain)*(int(self.Tx_Mode==0)) + (self.MicGain)*(int(self.Tx_Mode==1)) + (self.AMMIC/10.0)*(int(self.Tx_Mode==5)) ))
        self.blocks_multiply_const_vxx_3.set_k(self.Tx_Mode==4)
        self.blocks_multiply_const_vxx_4.set_k((self.Tx_Mode < 4) or (self.Tx_Mode==5))
        self.blocks_mute_xx_0_0_0.set_mute(bool((not self.PTT) or (self.Tx_Mode==2 and not self.KEY) or (self.Tx_Mode==3 and not self.KEY)))

    def get_Tx_LO(self):
        return self.Tx_LO

    def set_Tx_LO(self, Tx_LO):
        self.Tx_LO = Tx_LO
        self.iio_pluto_sink_0.set_frequency(self.Tx_LO)

    def get_Tx_Gain(self):
        return self.Tx_Gain

    def set_Tx_Gain(self, Tx_Gain):
        self.Tx_Gain = Tx_Gain
        self.iio_pluto_sink_0.set_attenuation(0,self.Tx_Gain)

    def get_Tx_Filt_Low(self):
        return self.Tx_Filt_Low

    def set_Tx_Filt_Low(self, Tx_Filt_Low):
        self.Tx_Filt_Low = Tx_Filt_Low
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 48000, self.Tx_Filt_Low, self.Tx_Filt_High, 100, window.WIN_HAMMING, 6.76))

    def get_Tx_Filt_High(self):
        return self.Tx_Filt_High

    def set_Tx_Filt_High(self, Tx_Filt_High):
        self.Tx_Filt_High = Tx_Filt_High
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 48000, self.Tx_Filt_Low, self.Tx_Filt_High, 100, window.WIN_HAMMING, 6.76))

    def get_ToneBurst(self):
        return self.ToneBurst

    def set_ToneBurst(self, ToneBurst):
        self.ToneBurst = ToneBurst
        self.analog_sig_source_x_1.set_amplitude((1.0*self.ToneBurst))

    def get_Rx_Mute(self):
        return self.Rx_Mute

    def set_Rx_Mute(self, Rx_Mute):
        self.Rx_Mute = Rx_Mute
        self.blocks_multiply_const_vxx_1.set_k(((self.AFGain/100.0) *  (not self.Rx_Mute)))

    def get_Rx_Mode(self):
        return self.Rx_Mode

    def set_Rx_Mode(self, Rx_Mode):
        self.Rx_Mode = Rx_Mode
        self.blocks_multiply_const_vxx_2.set_k(self.Rx_Mode<4)
        self.blocks_multiply_const_vxx_2_0.set_k(((self.Rx_Mode==4) * 0.2))
        self.blocks_multiply_const_vxx_2_1.set_k(self.Rx_Mode==5)
        self.blocks_multiply_const_vxx_2_1_0.set_k((1.0 + (self.Rx_Mode==5)))

    def get_Rx_LO(self):
        return self.Rx_LO

    def set_Rx_LO(self, Rx_LO):
        self.Rx_LO = Rx_LO
        self.iio_pluto_source_0.set_frequency(self.Rx_LO)

    def get_Rx_Gain(self):
        return self.Rx_Gain

    def set_Rx_Gain(self, Rx_Gain):
        self.Rx_Gain = Rx_Gain

    def get_Rx_Filt_Low(self):
        return self.Rx_Filt_Low

    def set_Rx_Filt_Low(self, Rx_Filt_Low):
        self.Rx_Filt_Low = Rx_Filt_Low
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 48000, self.Rx_Filt_Low, self.Rx_Filt_High, 100, window.WIN_HAMMING, 6.76))

    def get_Rx_Filt_High(self):
        return self.Rx_Filt_High

    def set_Rx_Filt_High(self, Rx_Filt_High):
        self.Rx_Filt_High = Rx_Filt_High
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 48000, self.Rx_Filt_Low, self.Rx_Filt_High, 100, window.WIN_HAMMING, 6.76))

    def get_RxOffset(self):
        return self.RxOffset

    def set_RxOffset(self, RxOffset):
        self.RxOffset = RxOffset
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.RxOffset)

    def get_PTT(self):
        return self.PTT

    def set_PTT(self, PTT):
        self.PTT = PTT
        self.blocks_mute_xx_0_0_0.set_mute(bool((not self.PTT) or (self.Tx_Mode==2 and not self.KEY) or (self.Tx_Mode==3 and not self.KEY)))

    def get_MicGain(self):
        return self.MicGain

    def set_MicGain(self, MicGain):
        self.MicGain = MicGain
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain)*(int(self.Tx_Mode==0)) + (self.MicGain)*(int(self.Tx_Mode==1)) + (self.AMMIC/10.0)*(int(self.Tx_Mode==5)) ))

    def get_KEY(self):
        return self.KEY

    def set_KEY(self, KEY):
        self.KEY = KEY
        self.blocks_mute_xx_0_0_0.set_mute(bool((not self.PTT) or (self.Tx_Mode==2 and not self.KEY) or (self.Tx_Mode==3 and not self.KEY)))

    def get_FMMIC(self):
        return self.FMMIC

    def set_FMMIC(self, FMMIC):
        self.FMMIC = FMMIC
        self.blocks_multiply_const_vxx_0_0.set_k((self.FMMIC/5.0))

    def get_FFT_SEL(self):
        return self.FFT_SEL

    def set_FFT_SEL(self, FFT_SEL):
        self.FFT_SEL = FFT_SEL
        self.blocks_keep_one_in_n_0.set_n((2 ** self.FFT_SEL))
        self.blocks_keep_one_in_n_0_0.set_n((2 ** self.FFT_SEL))
        self.logpwrfft_x_0.set_sample_rate((48000/ (2 ** self.FFT_SEL)))
        self.logpwrfft_x_0_0.set_sample_rate((48000/ (2** self.FFT_SEL)))
        _cutoff = 48000 / (2 ** (self.FFT_SEL + 1)) * 0.9 if self.FFT_SEL > 0 else 22000
        _trans = max(200, 48000 / (2 ** (self.FFT_SEL + 1)) * 0.1) if self.FFT_SEL > 0 else 2000
        self.fir_filter_xxx_fft_aa.set_taps(firdes.low_pass(1, 48000, _cutoff, _trans, window.WIN_HAMMING, 6.76))

    def get_CTCSS(self):
        return self.CTCSS

    def set_CTCSS(self, CTCSS):
        self.CTCSS = CTCSS
        self.analog_sig_source_x_1_0.set_frequency((self.CTCSS/10.0))
        self.analog_sig_source_x_1_0.set_amplitude((0.15 * (self.CTCSS >0)))

    def get_AMMIC(self):
        return self.AMMIC

    def set_AMMIC(self, AMMIC):
        self.AMMIC = AMMIC
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain)*(int(self.Tx_Mode==0)) + (self.MicGain)*(int(self.Tx_Mode==1)) + (self.AMMIC/10.0)*(int(self.Tx_Mode==5)) ))

    def get_AFGain(self):
        return self.AFGain

    def set_AFGain(self, AFGain):
        self.AFGain = AFGain
        self.blocks_multiply_const_vxx_1.set_k(((self.AFGain/100.0) *  (not self.Rx_Mute)))

    def _eq_coeffs(self, f0, dB_gain):
        """Biquad peaking EQ (Q=0.7) pour f0 Hz, gain en dB à fs=48kHz."""
        A     = 10.0 ** (dB_gain / 40.0)
        w0    = 2.0 * math.pi * f0 / 48000.0
        alpha = math.sin(w0) / (2.0 * 0.7)
        b0 =  1.0 + alpha * A;  b1 = -2.0 * math.cos(w0);  b2 = 1.0 - alpha * A
        a0 =  1.0 + alpha / A;  a1 = -2.0 * math.cos(w0);  a2 = 1.0 - alpha / A
        return [b0/a0, b1/a0, b2/a0], [1.0, a1/a0, a2/a0]

    def _comp_connect(self):
        # IIR HPF 1er ordre à ~300Hz (α=exp(-2π*300/48000)=0.9615)
        # oldstyle=False : rétroaction soustraite → y[n] = ff_sum - fb[1]*y[n-1]
        self.hpf_comp = filter.iir_filter_ffd(
            [0.9808, -0.9808], [1.0, -0.9615], False)
        # EQ 3 bandes paramétrables (biquad peaking, Q=0.7)
        # oldstyle=False requis : pôles stables avec coefficients biquad standard
        ff1, fb1 = self._eq_coeffs(self.comp_eq1_freq, self.comp_eq1_gain)
        self.eq1_comp = filter.iir_filter_ffd(ff1, fb1, False)
        ff2, fb2 = self._eq_coeffs(self.comp_eq2_freq, self.comp_eq2_gain)
        self.eq2_comp = filter.iir_filter_ffd(ff2, fb2, False)
        ff3, fb3 = self._eq_coeffs(self.comp_eq3_freq, self.comp_eq3_gain)
        self.eq3_comp = filter.iir_filter_ffd(ff3, fb3, False)
        # FIR LPF: limite la bande passante SSB
        self.lpf_comp = filter.fir_filter_fff(
            1,
            firdes.low_pass(0.9, 48000, self.comp_lpf_cutoff, 500,
                            window.WIN_HAMMING, 6.76))
        # AGC2: compresse un signal déjà filtré et égalisé
        self.agc2_comp = analog.agc2_ff(
            self.comp_agc_attack,
            self.comp_agc_decay,
            self.comp_agc_ref,
            1.0,
            self.comp_agc_max)
        self.blocks_multiply_const_vxx_0.set_k(self.comp_mic_gain / 100.0)
        self.disconnect((self.blocks_multiply_const_vxx_0, 0),
                        (self.blocks_add_const_vxx_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.hpf_comp, 0))
        self.connect((self.hpf_comp, 0),   (self.eq1_comp, 0))
        self.connect((self.eq1_comp, 0),   (self.eq2_comp, 0))
        self.connect((self.eq2_comp, 0),   (self.eq3_comp, 0))
        self.connect((self.eq3_comp, 0),   (self.lpf_comp, 0))
        self.connect((self.lpf_comp, 0),   (self.agc2_comp, 0))
        self.connect((self.agc2_comp, 0),  (self.blocks_add_const_vxx_0_0, 0))

    def _comp_disconnect(self):
        self.disconnect((self.blocks_multiply_const_vxx_0, 0), (self.hpf_comp, 0))
        self.disconnect((self.hpf_comp, 0),  (self.eq1_comp, 0))
        self.disconnect((self.eq1_comp, 0),  (self.eq2_comp, 0))
        self.disconnect((self.eq2_comp, 0),  (self.eq3_comp, 0))
        self.disconnect((self.eq3_comp, 0),  (self.lpf_comp, 0))
        self.disconnect((self.lpf_comp, 0),  (self.agc2_comp, 0))
        self.disconnect((self.agc2_comp, 0), (self.blocks_add_const_vxx_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0),
                     (self.blocks_add_const_vxx_0_0, 0))
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain)*(int(self.Tx_Mode==0)) + (self.MicGain)*(int(self.Tx_Mode==1)) + (self.AMMIC/10.0)*(int(self.Tx_Mode==5)) ))
        del self.hpf_comp
        del self.eq1_comp
        del self.eq2_comp
        del self.eq3_comp
        del self.lpf_comp
        del self.agc2_comp

    def set_COMP(self, enable):
        self.lock()
        if enable:
            self._comp_connect()
        else:
            self._comp_disconnect()
        self.unlock()

    def set_comp_param(self, param_id, raw_value):
        if   param_id == 0: self.comp_agc_attack  = raw_value / 1000.0
        elif param_id == 1: self.comp_agc_decay   = raw_value / 1000.0
        elif param_id == 2: self.comp_agc_ref      = raw_value / 100.0
        elif param_id == 3: self.comp_agc_max      = float(raw_value)
        elif param_id == 4: self.comp_lpf_cutoff   = raw_value
        elif param_id == 5: self.comp_eq1_freq     = raw_value
        elif param_id == 6: self.comp_eq1_gain     = raw_value / 10.0
        elif param_id == 7: self.comp_eq2_freq     = raw_value
        elif param_id == 8: self.comp_eq2_gain     = raw_value / 10.0
        elif param_id == 9: self.comp_eq3_freq     = raw_value
        elif param_id == 10: self.comp_eq3_gain    = raw_value / 10.0
        elif param_id == 11:
            self.comp_mic_gain = raw_value
            if hasattr(self, 'agc2_comp'):   # COMP is active, apply immediately
                self.blocks_multiply_const_vxx_0.set_k(self.comp_mic_gain / 100.0)

    def comp_apply(self):
        """Rebuild COMP chain with current params if active."""
        if hasattr(self, 'agc2_comp'):
            self.lock()
            self._comp_disconnect()
            self._comp_connect()
            self.unlock()

    def _nb1_connect(self):
        self.rational_resampler_nb_down = filter.rational_resampler_fff(
            interpolation=1, decimation=6, taps=[], fractional_bw=0)
        self.spectral_nr_0 = anr.spectral_nr_ff(
            fft_size=self.nb1_fft_size,
            overlap=self.nb1_overlap,
            algorithm=self.nb1_algorithm,
            alpha=self.nb1_alpha,
            beta=self.nb1_beta,
            gain_floor=self.nb1_gain_floor)
        self.rational_resampler_nb_up = filter.rational_resampler_fff(
            interpolation=6, decimation=1, taps=[], fractional_bw=0)
        self.disconnect((self.low_pass_filter_0, 0), (self.audio_sink_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.rational_resampler_nb_down, 0))
        self.connect((self.rational_resampler_nb_down, 0), (self.spectral_nr_0, 0))
        self.connect((self.spectral_nr_0, 0), (self.rational_resampler_nb_up, 0))
        self.connect((self.rational_resampler_nb_up, 0), (self.audio_sink_0, 0))

    def _nb1_disconnect(self):
        self.disconnect((self.low_pass_filter_0, 0), (self.rational_resampler_nb_down, 0))
        self.disconnect((self.rational_resampler_nb_down, 0), (self.spectral_nr_0, 0))
        self.disconnect((self.spectral_nr_0, 0), (self.rational_resampler_nb_up, 0))
        self.disconnect((self.rational_resampler_nb_up, 0), (self.audio_sink_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.audio_sink_0, 0))
        del self.rational_resampler_nb_down
        del self.spectral_nr_0
        del self.rational_resampler_nb_up

    def set_NB1(self, enable):
        self.lock()
        if enable:
            self._nb1_connect()
        else:
            self._nb1_disconnect()
        self.unlock()

    def set_nb1_param(self, param_id, raw_value):
        if   param_id == 0: self.nb1_algorithm  = raw_value
        elif param_id == 1: self.nb1_fft_size   = raw_value
        elif param_id == 2: self.nb1_overlap     = raw_value
        elif param_id == 3: self.nb1_alpha       = raw_value / 100.0
        elif param_id == 4: self.nb1_beta        = raw_value / 10.0
        elif param_id == 5: self.nb1_gain_floor  = raw_value / 1000.0

    def nb1_apply(self):
        """Rebuild NB1 chain with current params if active. Call after set_nb1_param()."""
        if hasattr(self, 'spectral_nr_0'):
            self.lock()
            self._nb1_disconnect()
            self._nb1_connect()
            self.unlock()

def docommands(tb):
  try:
    os.mkfifo("/tmp/langstoneTRx")
  except OSError as oe:
    if oe.errno != errno.EEXIST:
      raise    
  ex=False
  lastbase=0
  while not ex:
    fifoin=open("/tmp/langstoneTRx",'r')
    while True:
       try:
        with fifoin as filein:
         for line in filein:
           line=line.strip()
           if line[0]=='Q':
              ex=True                  
           if line[0]=='U':
              value=int(line[1:])
              tb.set_Rx_Mute(value)
           if line[0]=='H':
              value=int(line[1:])
              if value==1:   
                  tb.lock()
              if value==0:
                  tb.unlock() 
           if line[0]=='O':
              value=int(line[1:])
              tb.set_RxOffset(value)  
           if line[0]=='V':
              value=int(line[1:])
              tb.set_AFGain(value)
           if line[0]=='L':
              value=int(line[1:])
              tb.set_Rx_LO(value)
           if line[0]=='A':
              value=int(line[1:])
              tb.set_Rx_Gain(value) 
           if line[0]=='F':
              value=int(line[1:])
              tb.set_Rx_Filt_High(value) 
           if line[0]=='I':
              value=int(line[1:])
              tb.set_Rx_Filt_Low(value) 
           if line[0]=='M':
              value=int(line[1:])
              tb.set_Rx_Mode(value) 
              tb.set_Tx_Mode(value)
           if line=='R':
              tb.set_PTT(False) 
           if line=='T':
              tb.set_PTT(True)
           if line[0]=='K':
              value=int(line[1:])
              tb.set_KEY(value) 
           if line[0]=='B':
              value=int(line[1:])
              tb.set_ToneBurst(value) 
           if line[0]=='G':
              value=int(line[1:])
              tb.set_MicGain(value) 
           if line[0]=='g':
              value=int(line[1:])
              tb.set_FMMIC(value)
           if line[0]=='d':
              value=int(line[1:])
              tb.set_AMMIC(value)
           if line[0]=='f':
              value=int(line[1:])
              tb.set_Tx_Filt_High(value) 
           if line[0]=='i':
              value=int(line[1:])
              tb.set_Tx_Filt_Low(value)     
           if line[0]=='l':
              value=int(line[1:])
              tb.set_Tx_LO(value)  
           if line[0]=='a':
              value=int(line[1:])
              tb.set_Tx_Gain(value)     
           if line[0]=='C':
              value=int(line[1:])
              tb.set_CTCSS(value)   
           if line[0]=='W':
              value=int(line[1:])
              tb.set_FFT_SEL(value)
           if line[0]=='N':
              value=int(line[1:])
              if value == 2: tb.nb1_apply()
              else: tb.set_NB1(value)
           if line[0]=='c':
              value=int(line[1:])
              if value == 2: tb.comp_apply()
              else: tb.set_COMP(value)
           if line[0]=='s':
              tb.set_comp_param(0, int(line[1:]))
           if line[0]=='t':
              tb.set_comp_param(1, int(line[1:]))
           if line[0]=='u':
              tb.set_comp_param(2, int(line[1:]))
           if line[0]=='v':
              tb.set_comp_param(3, int(line[1:]))
           if line[0]=='w':
              tb.set_comp_param(4, int(line[1:]))
           if line[0]=='h':
              tb.set_comp_param(5, int(line[1:]))
           if line[0]=='k':
              tb.set_comp_param(6, int(line[1:]))
           if line[0]=='m':
              tb.set_comp_param(7, int(line[1:]))
           if line[0]=='n':
              tb.set_comp_param(8, int(line[1:]))
           if line[0]=='o':
              tb.set_comp_param(9, int(line[1:]))
           if line[0]=='q':
              tb.set_comp_param(10, int(line[1:]))
           if line[0]=='r':
              tb.set_comp_param(11, int(line[1:]))
           if line[0]=='e':
              tb.set_nb1_param(0, int(line[1:]))
           if line[0]=='j':
              tb.set_nb1_param(1, int(line[1:]))
           if line[0]=='p':
              tb.set_nb1_param(2, int(line[1:]))
           if line[0]=='x':
              tb.set_nb1_param(3, int(line[1:]))
           if line[0]=='y':
              tb.set_nb1_param(4, int(line[1:]))
           if line[0]=='z':
              tb.set_nb1_param(5, int(line[1:]))
                                                                                
       except:
         break



def main(top_block_cls=Lang_TRX_Pluto, options=None):
    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
