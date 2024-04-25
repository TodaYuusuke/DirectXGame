### --- テクスチャの扱い方 ---
１．textureフォルダをこのディレクトリに作成し、中にテクスチャファイルを入れる  
２．LWP\::Resource::LoadTexture関数で、テクスチャを読み込む  
- 返り値：LWP\::Resource::Textureクラスのポインタ）
- 引数：resources/texture直下からのパスを指定  
（resources/texture/Sample.pngならば、引数に指定するのは"Smaple.png"）
- 備考：LWP\::Resource::LoadTextureLongPathで、プロジェクトおよびexeからのパスで指定できる

### --- オーディオの扱い方 ---
１．audioフォルダをこのディレクトリに作成し、中にオーディオファイルを入れる  
２．LWP\::Resource::LoadAudio関数で、オーディオを読み込む  
- 返り値：LWP\::Resource::Audioクラスのポインタ）
- 引数：resources/audio直下からのパスを指定  
（resources/audio/Sample.wavならば、引数に指定するのは"Sample.wav"）
- 備考：LWP\::Resource::LoadAudioLongPathで、プロジェクトおよびexeからのパスで指定できる

### --- 3Dモデルの扱い方 ---
１．objフォルダをこのディレクトリに作成し、中に3Dモデルファイルを入れる  
２．LWP\::Resource::LoadModel関数で、3Dモデルを読み込む  
- 返り値：LWP\::Primitve::Meshクラスのポインタ）
- 引数：resources/texture直下からのパスを指定  
（resources/texture/Sample.objならば、引数に指定するのは"Smaple.obj"）
- 備考：LWP\::Resource::LoadModelLongPathは未実装