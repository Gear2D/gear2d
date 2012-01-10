/**
 * @file audiosource.cc
 * @b family audiosource
 * @b type audiosource
 * @author Leonardo Guilherme de Freitas
 * @addtogroup audiosource
 * 
 * Provides sound and music play/stop 
 *
 * @b provides
 * @li @c audiosource.sounds List of sounds to be loaded in the form id=file @b string
 * @li <tt>\<id\>.playing</tt> Sets the play/pause state of this particular sound. Each time you set it true, it will play. Defaults to false @b bool 
 * @li <tt>\<id\>.loops</tt> How many loops the next play should do
 * 
 * @b globals
 * @li @c audiosource.music File for the background music @b string
 * @li @c audiosource.music.playing Set true to play the music, set false to stop. Defaults to true. @b bool (0/1)
 */

#include "gear2d.h"
using namespace gear2d;

#include <string>
using namespace std;

#include "SDL.h"
#include "SDL_mixer.h"

struct sound {
	Mix_Chunk * raw;
	int loops;
	bool shallplay;
	set<int> channel;
	string id;
	
	// sounds that are playing
	static vector<sound *> playing;

	sound(Mix_Chunk * raw, int loops = 0)
	: raw(raw)
	, loops(loops)
	, shallplay(false)
	{ }
	
	void play() {
		int ch = Mix_PlayChannel(-1, raw, loops);
		if (ch == -1) {
			// stop the first channel
			Mix_HaltChannel(*channel.begin());
			ch = Mix_PlayChannel(-1, raw, loops);
		}
		if (ch == -1) return; // failed so bad...
		channel.insert(ch);
		playing[ch] = this;
		Mix_ChannelFinished(channel_finished);
	}
	
	void stop() {
		while (!channel.empty()) {
			Mix_HaltChannel(*channel.begin());
			// this should call finished(), hopefully.
		}
	}
	
	void finished(int ch) {
		channel.erase(ch);
	}
	
	static void channel_finished(int ch) {
		sound * p = playing[ch];
		if (p != 0) p->finished(ch);
		playing[ch] = 0;
	}
};

vector<sound *> sound::playing(32, (sound *)0);

class audiosource : public gear2d::component::base {
	private:
		static bool initialized;
		static bool musicisplaying;
		static string playingmusic;
		static string musictoplay;
		static string sndpath;
		static string muspath;
		static map<string, Mix_Music *> musics;
		static map<string, Mix_Chunk *> rawbyfile;
		
	private:
		map<string, sound *> soundbyid;
		
	public:
        virtual component::type type() { return "audiosource"; }
        virtual component::family family() { return "audiosource"; }
		
		virtual void handle(parameterbase::id pid, component::base * last, object::id owner) {
			if (pid == "audiosource.music") {
				cout << musictoplay << " beach  " << playingmusic << endl;
				if (musictoplay != playingmusic) {
					if (musicisplaying) playmusic(musictoplay, -1);
				}
				return;
			}
			
			if (pid == "audiosource.sounds") {
				loadsounds(read<string>("audiosource.sounds"));
				return;
			}
			
			if (pid == "audiosource.music.playing") {
				if (musicisplaying == false) stopmusic();
				return;
			}
			
			int p;
			if ((p = pid.find(".playing")) != string::npos) {
				sound *& snd = soundbyid[pid.substr(0, p)];
				if (snd == 0) return;
				if (snd->shallplay) // shallplay is binded to .playing so it was already written
					snd->play();
			}
		}

		virtual void setup(object::signature & sig) {
			initialize(sig["audiosource.music"], sig["sndpath"], sig["muspath"], eval(sig["audiosource.music.playing"], true));
			
			bind("audiosource.music", musictoplay);
			bind("audiosource.music.playing", musicisplaying);
			
			
			hook("audiosource.music");
			hook("audiosource.music.playing");
			write("audiosource.music", sig["audiosource.music"]);
			write("audiosource.music.playing", eval(sig["audiosource.music.playing"], true));
			
			
			write<string>("audiosource.sounds", sig["audiosource.sounds"]);
			hook("audiosource.sounds");
			
			loadsounds(sig["audiosource.sounds"]);
			
			for (map<string, sound *>::iterator snd = soundbyid.begin(); snd != soundbyid.end(); snd++) {
				string id = snd->first;
				sound * sound = snd->second;
				write(id + ".loops", eval(sig[id + ".loops"], 0));
				write(id + ".playing", eval(sig[id + ".playing"], false));
			}
		}
		virtual void update(float dt) {
			
		}
		
	private:
		static void initialize(string initialmusic, string soundpath, string musicpath, bool shouldplay) {
			if (initialized) return;
			if (!SDL_WasInit(SDL_INIT_AUDIO)) {
				if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
					std::cerr << "(audiosource component): Error initializing audio subsystem: " << SDL_GetError() << std::endl;
					return;
				}
				
				/*if (Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
					std::cerr << "(audiosource component): Unable to initialize any music format. You will not be able to play anything." << endl;
					return;
				}*/
				
				if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0) {
					std::cerr << "(audiosource component): Unable to open the audio subsystem" << endl;
					return;
				}
			}
			sndpath = soundpath;
			muspath = musicpath;
			if (initialmusic != "") {
				playingmusic = initialmusic;
				if (shouldplay) playmusic(playingmusic, -1);
			}
			initialized = true;
		}
		
		void loadsounds(string soundlist) {
			if (soundlist == "") return;
			set<string> sounds;
			split(sounds, soundlist, ' ');
			for (set<string>::iterator sounddef = sounds.begin(); sounddef != sounds.end(); sounddef++) {
				int p = sounddef->find('=');
				string id = sounddef->substr(0, p);
				string file = sounddef->substr(p+1);
				if (soundbyid[id] != 0) continue;
				Mix_Chunk * chunk = getsound(sndpath + file);
				sound * snd = new sound(chunk);
				soundbyid[id] = snd;
				bind(id + ".playing", snd->shallplay);
				hook(id + ".playing");
				bind(id + ".loops", snd->loops);
			}
		}
		
		static Mix_Chunk * getsound(string soundfn) {
			Mix_Chunk *& chunk = rawbyfile[soundfn];
			if (chunk != 0) return chunk;
			chunk = Mix_LoadWAV(soundfn.c_str());
			if (chunk == 0) {
				std::cerr << "(audiosource component): Unable to load " << soundfn << ": " << SDL_GetError() << endl;
				return 0;
			}
			
			return chunk;
		}
		
		static Mix_Music * loadmusic(string musicfn) {
			Mix_Music *& m = musics[musicfn];
			if (m == 0) {
				m = Mix_LoadMUS(musicfn.c_str());
				if (m == 0) {
					std::cerr << "(audiosource component): Unable to load music " << musicfn << ": " << SDL_GetError() << std::endl;
					return 0;
				}
			}
			return m;
		}
		
		static void playmusic(string musicfn, int loops) {
			Mix_Music * m = loadmusic(muspath + musicfn);
			if (m == 0) return;
			if (Mix_PlayingMusic() == 1) {
			}
			Mix_PlayMusic(m, loops);
			musicisplaying = true;
		}
		
		static void stopmusic() {
			Mix_HaltMusic();
			musicisplaying = false;
		}
		
};

bool audiosource::initialized = false;
bool audiosource::musicisplaying = false;
string audiosource::playingmusic;
string audiosource::musictoplay;
string audiosource::sndpath;
string audiosource::muspath;
map<string, Mix_Music *> audiosource::musics;
map<string, Mix_Chunk *> audiosource::rawbyfile;

extern "C" {
	component::base * build() { return new audiosource; }
}
