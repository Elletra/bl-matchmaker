package ArrangedOnly { function ConnectToServer (%address, %password, %useDirect, %useArranged) { Parent::ConnectToServer (%address, %password, false, true); } }; activatePackage (ArrangedOnly); setMatchMakerIP ("45.79.69.119:5555");