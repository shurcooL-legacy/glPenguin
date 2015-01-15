package main

var config struct {
	General struct {
		ShowIntro         bool
		TestFallingStairs bool
	}

	Video struct {
		Fullscreen  bool
		ResolutionX int32
		ResolutionY int32
		ColourBits  int32
		DepthBits   int32
	}

	World struct {
		WorldX             int32
		WorldY             int32
		WinHeight          int32
		WorldTickTime      float32
		IceCubeRespawnRate float32
	}
}
