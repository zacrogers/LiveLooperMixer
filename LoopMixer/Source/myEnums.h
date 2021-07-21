#pragma once

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
		STOPPED
	};
};

