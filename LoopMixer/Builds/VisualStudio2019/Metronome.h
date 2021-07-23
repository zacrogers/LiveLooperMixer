#pragma once
#include <JuceHeader.h>
#include <cstdint>

class Metro : public Timer
{
public:
	enum Subdivision
	{
		WHOLE = 1,
		HALF = 2,
		THIRD = 3,
		QUARTER = 4,
		EIGHTH = 8,
		SIXTH = 16
	};

	enum class TimeSig
	{
		_5_4,
		_4_4,
		_3_4,
		_2_4
	};


	Metro(int tempo, float vol) : 
		_tempo(tempo), 
		_vol(vol), 
		_isMuted(false),
		_isPlaying(false),
		_subdiv(QUARTER),
		_timesig(TimeSig::_4_4)
	{}
	void timerCallback() override;

	bool isMuted(void) const { return _isMuted; }
	void setMuted(bool muted) { _isMuted = muted; }

	bool isPlaying(void) const { return _isPlaying; }

	void play(void);
	void stop(void);

	void setVolume(float vol) { _vol = vol; }
	void setTempo(int tempo) { _tempo = tempo; }
	void setSubdiv(Subdivision subdiv) { _subdiv = subdiv; }
	void setTimesig(TimeSig timesig) { _timesig = timesig; }

private:
	bool _isMuted;
	bool _isPlaying;
	int _tempo;
	float _vol;
	Subdivision _subdiv;
	TimeSig _timesig;

	static std::uint8_t bpmToMs(std::uint8_t bpm, Subdivision subdivision) { return 60000 / bpm * subdivision; }

};