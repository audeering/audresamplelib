import audiofile as af
import sox
import numpy as np

channel_list = [1, 2]
n_samples_generated = 100
sr_list = [8000, 16000, 44100]  # Hz
peak = 0.8
sine_freq = 1000  # Hz

# generate noise
noise_array = 2 * peak * np.random.rand(2 * n_samples_generated) - peak

for n_ch in channel_list:
    for sr_in in sr_list:
        for signal_type in ['noise', 'sine']: 
            wav_original = f'original-{signal_type}__sr_{sr_in}__channels_{n_ch}.wav'
	    
            if signal_type is 'noise':
                x = noise_array[:n_ch * n_samples_generated]        
            elif signal_type is 'sine':
                t = np.arange(n_samples_generated) / sr_in
                x = peak * np.sin(2 * np.pi * sine_freq * t)
                if n_ch > 1:
                    x_extension = peak * np.cos(2 * np.pi * sine_freq * t)
                    x = np.append(x, x_extension)

            af.write(wav_original, np.reshape(x, (n_ch, -1)), sr_in)

            for sr_out in sr_list:
                if sr_out == sr_in:
                    continue
                wav_resampled = f'resampled-{signal_type}__sr-in_{sr_in}__sr-out_{sr_out}__channels_{n_ch}.wav'
                tfm = sox.Transformer()
                tfm.channels(1)
                tfm.rate(sr_out)
                tfm.build(wav_original, wav_resampled)
