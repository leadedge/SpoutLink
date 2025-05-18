## SpoutLink

Spout texture sharing for Windows uses DirectX 11 (D3D11), shared textures that allow GPU resources to be shared between separate applications.

The texture formats that can be shared include 8 bit BGRA and RGBA, 10 bit RGBA and 16 bit or 32 bit float RGBA. The default used for Spout applications is BGRA (DXGI_FORMAT_B8G8R8A8_UNORM).

Compatibility for these formats requires both processes to be based on DirectX 11. It is possible to share textures between DirectX 11 and DirectX 9, with two compatible formats, BGRA (with alpha) and BGRX (alpha disregarded).

- DXGI_FORMAT_B8G8R8A8_UNORM
- DXGI_FORMAT_B8G8R8X8_UNORM

Applications based on DirectX 9 can receive Spout shared textures with only these two formats. For example, a sender application generating shared textures with RGBA format (DXGI_FORMAT_R8G8B8A8_UNORM) is incompatible with a DirectX 9 Spout receiver.

SpoutLink is designed to overcome this problem by receiving in all possible formats and sending in the default BGRA (DXGI_FORMAT_B8G8R8A8_UNORM) format which is compatible with DirectX 9.

Open SpoutLink, and select the sender application required. In the DirectX 9 receiving application, select the sender named "SpoutLink" which is a direct copy of the original but with a compatible format.

### Building the project

The project depends on :

[ofxWinMenu](https://github.com/leadedge/ofxWinMenu)
[Spout SDK](https://github.com/leadedge/Spout2/tree/master/SPOUTSDK/SpoutGL)

The source files of the Spout SDK are not set up as an addon, so they must be copied to the "SpoutGL" folder within he Openframeworks folder as follows :

<pre>
C:\OPENFRAMEWORKS
	addons
		ofxWinMenu <- download the ofxWinMenu addon and copy to the addons folder
	apps
		SpoutGL <- create this folder (under "apps") and copy the SpoutGL source files into it
		myApps
			SpoutLink <- copy the entire SpoutLink folder here (under "apps\myApps")
				icon.rc
				SpoutLink.sln
				SpoutLink.vcxproj
				SpoutLink.vcxproj.filters
				src <- source files
				bin <- data files
</pre>

Open the solution file SpoutLink.sln with Visual Studio 2022
Set to "Release" "x64" and build "SpoutLink".


