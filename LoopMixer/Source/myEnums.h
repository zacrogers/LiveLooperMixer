#pragma once


enum Subdivision
{
	WHOLE = 1,
	QUARTER = 4,
	EIGHTH = 8,
	SIXTH = 16
};

namespace State
{
	enum class Transport
	{
		STOPPED,
		STARTING,
		PLAYING,
		STOPPING
	};


	enum class Track
	{
		PLAYING,
		QUEUED,
		STOPPING,
		STOPPED
	};
};

