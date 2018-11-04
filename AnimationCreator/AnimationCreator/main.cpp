#include <iostream>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <boost/algorithm/string.hpp>
#include <tinyxml.h>

using namespace std;

struct point
{
	unsigned int x;
	unsigned int y;
};

struct anim_data
{
	anim_data()
	{
		frames_number = 0;
	}

	unsigned int frames_number;
	vector<sf::Image> frames;
	vector<unsigned int> delays;
	vector<point> hot_points;
	vector< vector<sf::IntRect> > defense_boxes;
	vector< vector<sf::IntRect> > offense_boxes;
	string spritesheet;
	vector<sf::IntRect> frames_positions;
};

bool load_anim(string dir, anim_data& anim)
{
	cout << "Load animation " << dir << endl;

	//anim.txt
	ifstream ifs(string(dir + string("/anim.txt")).c_str());

	if (ifs.bad())
	{
		cout << "Can't open " << dir << "/anim.txt" << endl;
		return false;
	}

	cout << "Reading anim.txt..." << endl;
	string line;

	while (getline(ifs, line))
	{
		if (line.find("sprites=") == 0)
		{
			vector<string> v;
			boost::split(v, line, boost::is_any_of("="));

			istringstream iss(v[1]);
			iss >> anim.frames_number;
			anim.hot_points.resize(anim.frames_number);
			anim.defense_boxes.resize(anim.frames_number);
			anim.offense_boxes.resize(anim.frames_number);

			cout << "\tframes : " << anim.frames_number << endl;
			anim.frames_positions.resize(anim.frames_number);
		}

		else if (line.find("delays=") == 0)
		{
			vector<string> v;
			boost::split(v, line, boost::is_any_of("="));

			vector<string> delays;
			boost::split(delays, v[1], boost::is_any_of(","));

			vector<string>::iterator it;
			for (it = delays.begin(); it != delays.end(); ++it)
			{
				istringstream iss(*it);
				unsigned int delay = 0;
				iss >> delay;
				anim.delays.push_back(delay);
			}

			cout << "\tdelays : " << v[1] << endl;
		}
	}

	ifs.close();

	// collisions.txt
	unsigned int current_frame = 0;

	ifs.open(string(dir + string("/collisions.txt")).c_str());

	if (ifs.bad())
	{
		cout << "Can't open " << dir << "/collisions.txt" << endl;
		return false;
	}

	cout << "Reading collisions.txt..." << endl;

	while (getline(ifs, line))
	{
		if (line.find("Frame ") == 0)
		{
			vector<string> v;
			boost::split(v, line, boost::is_any_of(" "), boost::token_compress_on);

			istringstream iss(v[1]);
			iss >> current_frame;

			cout << "\tReading data of frame " << current_frame << endl;
		}
		else if (line.find("hotPoint ") == 0)
		{
			vector <string> v;
			boost::split(v, line, boost::is_any_of(" "), boost::token_compress_on);

			istringstream iss(v[1] + " " + v[2]);
			point p;
			iss >> p.x;
			iss >> p.y;

			if (current_frame > 0 && current_frame <= anim.frames_number)
			{
				anim.hot_points[current_frame - 1] = p;
				cout << "\t\thotPoint x : " << p.x << " y : " << p.y << endl;
			}
		}
		else if (line.find("DEFENSE ") == 0)
		{
			vector <string> v;
			boost::split(v, line, boost::is_any_of(" "), boost::token_compress_on);

			vector<string> box;
			boost::split(box, v[1], boost::is_any_of(","));

			sf::IntRect b;
			istringstream iss(box[0] + " " + box[1] + " " + box[2] + " " + box[3]);
			iss >> b.left;
			iss >> b.top;
			iss >> b.width;
			iss >> b.height;

			if (current_frame > 0 && current_frame <= anim.frames_number)
			{
				anim.defense_boxes[current_frame - 1].push_back(b);
				cout << "\t\tDefense Box -> left : " << b.left << " top : " << b.top << " width : " << b.width << " height : " << b.height << endl;
			}
		}
		else if (line.find("OFFENSE ") == 0)
		{
			vector <string> v;
			boost::split(v, line, boost::is_any_of(" "), boost::token_compress_on);

			vector<string> box;
			boost::split(box, v[1], boost::is_any_of(","));

			sf::IntRect b;
			istringstream iss(box[0] + " " + box[1] + " " + box[2] + " " + box[3]);
			iss >> b.left;
			iss >> b.top;
			iss >> b.width;
			iss >> b.height;

			if (current_frame > 0 && current_frame <= anim.frames_number)
			{
				anim.offense_boxes[current_frame - 1].push_back(b);
				cout << "\t\tOffense Box -> left : " << b.left << " top : " << b.top << " width : " << b.width << " height : " << b.height << endl;
			}
		}
	}

	ifs.close();

	cout << "Loading frames..." << endl;

	//load frames
	for (unsigned int i = 1; i <= anim.frames_number; i++)
	{
		ostringstream oss(ostringstream::out);
		oss << dir << "/" << i << ".png";
		sf::Image img;
		if (!img.loadFromFile(oss.str()))
		{
			cout << "\tCan't load " << oss.str() << endl;
			return false;
		}

		anim.frames.push_back(img);
		cout << "\t" << oss.str() << " loaded." << endl;
	}

	cout << "Animation loaded." << endl;

	return true;
}

bool anim_to_xml(anim_data& anim, string file)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
	doc.LinkEndChild(decl);

	TiXmlElement * animation = new TiXmlElement("animation");
	animation->SetAttribute("file", (anim.spritesheet + ".png").c_str());

	for (unsigned int i = 0; i < anim.frames_number; i++)
	{
		TiXmlElement * frame = new TiXmlElement("frame");
		frame->SetAttribute("delay", anim.delays[i]);
		frame->SetAttribute("x", anim.frames_positions[i].left);
		frame->SetAttribute("y", anim.frames_positions[i].top);
		frame->SetAttribute("width", anim.frames[i].getSize().x);
		frame->SetAttribute("height", anim.frames[i].getSize().y);
		frame->SetAttribute("hotpointx", anim.hot_points[i].x);
		frame->SetAttribute("hotpointy", anim.hot_points[i].y);

		vector<sf::IntRect>::iterator it;
		for (it = anim.defense_boxes[i].begin(); it != anim.defense_boxes[i].end(); ++it)
		{
			TiXmlElement * defense_box = new TiXmlElement("defense");
			defense_box->SetAttribute("x", it->left);
			defense_box->SetAttribute("y", it->top);
			defense_box->SetAttribute("width", it->width);
			defense_box->SetAttribute("height", it->height);

			frame->LinkEndChild(defense_box);
		}

		for (it = anim.offense_boxes[i].begin(); it != anim.offense_boxes[i].end(); ++it)
		{
			TiXmlElement * offense_box = new TiXmlElement("offense");
			offense_box->SetAttribute("x", it->left);
			offense_box->SetAttribute("y", it->top);
			offense_box->SetAttribute("width", it->width);
			offense_box->SetAttribute("height", it->height);

			frame->LinkEndChild(offense_box);
		}

		animation->LinkEndChild(frame);
	}

	doc.LinkEndChild(animation);
	return doc.SaveFile(file.c_str());
}

struct options
{
	bool all;

	options()
	{
		all = false;
	}
};

int doAnim(char* dirname, char* spritesheetname)
{
	string dir_name(dirname), spritesheet_name(spritesheetname);

	cout << "Directory name : " << dir_name << endl << endl;

	//open each image and compute size of final spritesheet
	unsigned int sheet_width = 0, sheet_height = 0, max_frame_height = 0, current_width = 0;
	anim_data anim;
	anim.spritesheet = spritesheet_name;

	if (!load_anim(dir_name, anim))
	{
		cout << "Failed to load anim " << dir_name << endl;
		return EXIT_FAILURE;
	}

	//compute max height of one frame
	vector<sf::Image>::iterator it;
	for (it = anim.frames.begin(); it != anim.frames.end(); ++it)
	{
		if (max_frame_height < it->getSize().y) max_frame_height = it->getSize().y;
	}

	//now compute size of spritesheet (max width is 512)
	sheet_height = max_frame_height;

	for (it = anim.frames.begin(); it != anim.frames.end(); ++it)
	{
		if (current_width + it->getSize().x > 512)
		{
			sheet_height += max_frame_height;
			if (sheet_width < current_width) sheet_width = current_width;
			current_width = 0;
		}
		else
		{
			current_width += it->getSize().x;
		}
	}

	if (sheet_width == 0) sheet_width = current_width;

	//create spritesheet
	cout << endl << "Create spritesheet..." << endl;
	sf::Image sheet;
	sheet.create(sheet_width, sheet_height, sf::Color(255, 255, 255, 0));

	cout << "Spritesheet size : " << sheet_width << "x" << sheet_height << endl;

	//copy pixels of frames into the sheet
	unsigned int destY = 0;
	for (unsigned int destX = 0, i = 0; i < anim.frames.size() /*destX < sheet_width*/; i++)
	{
		sf::Image &img = anim.frames[i];

		sheet.copy(img, destX, destY, sf::IntRect(0, 0, 0, 0), true);

		//update frame position with position on spritesheet (used later to write xml)
		anim.frames_positions[i].left = destX;
		anim.frames_positions[i].top = destY;
		anim.frames_positions[i].width = img.getSize().x;
		anim.frames_positions[i].height = img.getSize().y;

		destX += img.getSize().x;

		if ((i < anim.frames.size() - 1) && (destX + anim.frames[i + 1].getSize().x > sheet_width))
		{
			destX = 0;
			destY += max_frame_height;
		}
	}

	//save sheet
	if (!sheet.saveToFile(dir_name + "/" + spritesheet_name + ".png")) {
		std::cout << "Failed to save spritesheet." << endl;
		return EXIT_FAILURE;
	}

	std::cout << "Spritesheet has been created successfully." << endl;

	if (!anim_to_xml(anim, dir_name + "/" + spritesheet_name + ".xml"))
	{
		std::cout << "Failed to save data in " << dir_name << "/" << spritesheet_name << ".xml" << endl;
		return EXIT_FAILURE;
	}

	std::cout << "Saved data in " << dir_name << "/" << spritesheet_name << ".xml" << endl;

	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	cout << "********************************" << endl;
	cout << "***     Animation Creator    ***" << endl;
	cout << "********************************" << endl;

	//parse options
	options ops;
	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);

		//argument is option ?
		if (arg.find_first_of('-') == 0)
		{
			//option -all
			if (arg.compare("-all") == 0)
			{
				ops.all = true;
			}
		}
	}

	if (argc < 2) {
		cout << "Some arguments are missing." << endl;
		cout << "Usage : <program_name> <options> <dir_name> <spritesheet>" << endl;
		return EXIT_FAILURE;
	}

	int ret = 0;

	if (ops.all)
	{
		for (int i = 1; i < argc; i++)
		{
			string arg(argv[i]);

			//if argument is option, ignore it
			if (arg.find_first_of('-') == 0) continue;

			ret = doAnim(argv[i], argv[i]);
			if (ret != EXIT_SUCCESS)
				return ret;
		}
	}
	else
	{
		if (argc < 3)
		{
			cout << "Usage without option -all : some arguments are missing." << endl;
			cout << "Usage : <program_name> <options> <dir_name> <spritesheet>" << endl;
			return EXIT_FAILURE;
		}

		ret = doAnim(argv[1], argv[2]);
	}

	return ret;
}