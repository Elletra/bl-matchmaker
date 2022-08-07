function ConnectArrangedOnly (%address, %password)
{
	Connecting_Text.setText ("Connecting to " @ %address);
	Canvas.pushDialog (connectingGui);

	ConnectToServer (%address, %password, false, true);
}
