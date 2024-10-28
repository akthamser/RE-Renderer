#include"ScreenRecorder.h"




ScreenRecorder::ScreenRecorder(int width, int height, int frames, const std::string& fileName,bool overRide) : m_width(width), m_height(height) {

	int result = system("ffmpeg -version");
	if (result != 0)
	{
		std::cout << "ffmpeg not installed properly , make sure its added to your PATH envirment variables" << std::endl;
		return;
	}
	std::string override = overRide ? "-y" : "-n";
	commande = "ffmpeg "+ override +" -f rawvideo -pix_fmt bgr24 -s "
		+ std::to_string(width) + "x" + std::to_string(height) +
		" -r " + std::to_string(frames) +
		" -i - -vf vflip -c:v libx264 -preset fast -crf 18 -pix_fmt yuv420p " + fileName;


	
};

void ScreenRecorder::StartRecording() {

	ffmpegPipe = _popen(commande.c_str(), "wb");
	if (!ffmpegPipe)
	{
		std::cout << "cant open ffmpeg pipe " << std::endl;
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

ScreenRecorder::~ScreenRecorder() {
	
	if (!ffmpegPipe) {
		std::cout << "FFmpeg pipe was not opened." << std::endl;
	}
	else {
		_pclose(ffmpegPipe);
	}
}

void ScreenRecorder::captureFrame() {

	std::vector<unsigned char> pixels(m_width * m_height * 3);

	glReadPixels(0, 0, m_width, m_height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

	size_t written = fwrite(reinterpret_cast<const char*>(pixels.data()), sizeof(unsigned char), pixels.size(), ffmpegPipe);
	if (written != pixels.size()) {
		std::cerr << "Error writing to FFmpeg pipe!" + std::to_string(written) << std::endl;
	}
}