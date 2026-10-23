Musicus
=======
A simple music player to manage your music library.

Overview
--------
On startup Musicus will ask for a directory, where the music is stored. It then scans all audio files and creates the
library view based on it. The read metadata is stored in a file called `.metadata.musicuslib` placed in the 
selected library directory. Once the library is created you can select songs to play.

Dependencies
------------
- JUCE Framework (https://juce.com/)
- simdjson (https://simdjson.github.io/simdjson/index.html) The files are already included in the source code

Build
-----
In order to build `Musicus` you need to install a local copy of JUCE. You can clone the official JUCE
Github Repository or download a snapshot as a ZIP archive and extract it. Then open the Projucer in
your IDE, build and run it. The Projucer project file is located in `<your_path>/JUCE/extras/Projucer/builds/<system>/`.
For more information see the JUCE documentation.
With the Projucer open the `Musicus.jucer` and then click "save and open" in
IDE in the Projucer. After you did that you can build and run `Musicus`.