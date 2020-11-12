#include <iostream>
#include <vector>
#include <Windows.h>
#include "nkcOpenCV.h"
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Mono Image"
#define MONO_PROCESS 2

bool OpenFile(cv::String& filestring);

int main() {
#ifdef _DEBUG
	const cv::String filename = "vrs.jpg";
#else
	cv::String filename;
	if (!::OpenFile(filename)) return 1;
#endif

	// モノクロ処理
#if MONO_PROCESS == 1
	// 最初からモノクロで読み込む
	auto mono = cv::imread(filename, cv::IMREAD_GRAYSCALE);
#endif
#if MONO_PROCESS == 2
	// cvtColorで変換
	auto imgIn = cv::imread(filename);
	cv::Mat mono;
	cv::cvtColor(imgIn, mono, cv::COLOR_BGR2GRAY);
#endif
#if MONO_PROCESS == 3
	// チャネル分離
	auto imgIn = cv::imread(filename);
	std::vector<cv::Mat> imgs;
	cv::split(imgIn, imgs);
	auto mono = imgs[0].clone();	// ここでは先頭チャネル(Blue)を使う
#endif
	std::cout << "# of channels of mono: " << mono.channels() << std::endl;

	// ウインドウ表示
	cvui::init(WINDOW_NAME);
	const int MARGIN = 10;
	while (true) {
		cv::Mat frame(mono.rows + MARGIN * 2, mono.cols + MARGIN * 2, CV_8UC3, cv::Scalar(49, 52, 49));
		cv::Mat monoDisp;
		cv::cvtColor(mono, monoDisp, cv::COLOR_GRAY2BGR);
		cvui::image(frame, MARGIN, MARGIN, monoDisp);
		cvui::update();
		cv::imshow(WINDOW_NAME, frame);
		if (cv::waitKey(20) == 27) break;
	}

	return 0;
}

// ファイルを開く
// 引数 filestring：ファイル名を格納する文字列
// 戻り値：true:ファイルが指定された false:キャンセル
bool OpenFile(cv::String &filestring) {

	char path[MAX_PATH], filename[MAX_PATH];

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = path;     //選択されたファイル名を受け取る(フルパス)
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = filename; //選択されたファイル名
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrFilter = "JPEG(*.jpg)\0*.jpg\0\0"; //ファイル種類フィルタ
	ofn.lpstrTitle = "画像ファイルを選択してください"; //ダイアログボックスのタイトル
	if (::GetOpenFileName(&ofn) == FALSE) return false;
	filestring = filename;

	return true;
}
