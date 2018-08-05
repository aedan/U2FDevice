#include "Streams.hpp"
#include <iostream>

using namespace std;

FILE* initHTML(FILE *fPtr, const string &title);
void closeHTML(FILE *fPtr);

shared_ptr<FILE> getHostStream()
{
	static shared_ptr<FILE> stream{ fopen("/dev/hidg0", "ab+"), [](FILE *f){
			fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getComHostStream()
{
	static shared_ptr<FILE> stream{ fopen("comhost.txt", "wb"), [](FILE *f){
		clog << "Closing comhost stream" << endl;
		fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getHostPacketStream()
{
	static shared_ptr<FILE> stream{ initHTML(fopen("hostpackets.html", "wb"), "Host Packets"), [](FILE *f){
		clog << "Closing hostPackets stream" << endl;
		closeHTML(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getHostAPDUStream()
{
	static shared_ptr<FILE> stream{ initHTML(fopen("hostAPDU.html", "wb"), "Host APDU"), [](FILE *f){
		clog << "Closing host APDU stream" << endl;
		closeHTML(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getComDevStream()
{
	static shared_ptr<FILE> stream{ fopen("comdev.txt", "wb"), [](FILE *f){
		clog << "Closing comdev stream" << endl;	
		fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getDevPacketStream()
{
	static shared_ptr<FILE> stream{ initHTML(fopen("devpackets.html", "wb"), "Dev Packets"), [](FILE *f){
		clog << "Closing devPackets stream" << endl;
		closeHTML(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getDevAPDUStream()
{
	static shared_ptr<FILE> stream{ initHTML(fopen("devAPDU.html", "wb"), "Dev APDU"), [](FILE *f){
		clog << "Closing dev APDU stream" << endl;
		closeHTML(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

FILE* initHTML(FILE *fPtr, const string &title)
{
	fprintf(fPtr, "<html>\n"
				"\t<head>\n"
				"\t\t<title>%s</title>\n"
				"\t\t<style>\n"
				"\t\t\ttable {\n"
				"\t\t\t\tdisplay: table;\n"
				"\t\t\t\twidth: 100%;\n"
				"\t\t\t\tborder-collapse: collapse;\n"
				"\t\t\t\tbox-sizing: border-box;\n"
				"\t\t\t}\n"
				"\n"
				"\t\t\tth.data {\n"
				"\t\t\t\ttext-align: left;\n"
				"\t\t\t}\n"
				"\n"
				"\t\t\ttd {\n"
				"\t\t\t\tfont-family: \"Courier New\", Courier, monospace;\n"
				"\t\t\t\twhite-space: pre;\n"
				"\t\t\t}\n"
				"\n"
				"\t\t\ttd.data {\n"
				"\t\t\t\toverflow: hidden;\n"
				"\t\t\t\ttext-overflow: ellipsis;\n"
				"\t\t\t\tmax-width:1px;\n"
				"\t\t\t\twidth:100%;\n"
				"\t\t\t}\n"
				"\n"
				"\t\t\ttd.data:hover {\n"
				"\t\t\t\twhite-space: pre-wrap;\n"
				"\t\t\t}\n"
				"\n"
				"\t\t\ttable, th, td {\n"
				"\t\t\t\tborder: 1px solid black;\n"
				"\t\t\t}\n"
				"\t\t</style>\n"
				"\t</head>\n"
				"\n"
				"\t<body>", title.c_str());

	return fPtr;
}

void closeHTML(FILE *fPtr)
{
	fprintf(fPtr, "\t</body>\n"
				"</html>");
	fclose(fPtr);
}
