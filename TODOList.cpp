#include <SFML/Graphics.hpp>
#include <windows.h>
#include <fstream>
#include <string>
#include <codecvt>
#include <iostream>
using namespace sf;

#define width 640
#define height 480


int main()
{
	int offset = 0;

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	RenderWindow window(VideoMode(width, height), "TODO List");
	window.setFramerateLimit(60);

	std::wfstream file;
	file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	file.open("db.txt", std::ios_base::in);
	std::wstring buff;
	wchar_t buffer[256] = {0};
	file.getline(buffer, 256,'\n');
	buff = buffer;
	if(buff[0] == 65279) buff.erase(0, 1);
	int stage = 0;

	Texture main_texture[10];
	Texture tick[2];

	if (!main_texture[0].loadFromFile("files/main.jpg")) return 1;
	if (!main_texture[1].loadFromFile("files/main_clarified.jpg")) return 1;
	if (!main_texture[2].loadFromFile("files/1.jpg")) return 1;
	if (!main_texture[3].loadFromFile("files/1_without_passphrase.jpg")) return 1;
	if (!main_texture[4].loadFromFile("files/2.jpg")) return 1;
	if (!main_texture[5].loadFromFile("files/2_without_passphrase.jpg")) return 1;

	if (!tick[0].loadFromFile("files/tick0.png")) return 1;
	if (!tick[1].loadFromFile("files/tick1.png")) return 1;

	Texture cursor;
	cursor.loadFromFile("files/caret.png");

	Sprite main_sprite;
	if(buff.empty())
	{
		main_sprite.setTexture(main_texture[0]);
		file.close();
	}
	else
	{
		stage = 4;
		main_sprite.setTexture(main_texture[stage]);
		file.close();
	}
	Vector2i pos = {0,0};
	Vector2i coord_sym[128];

	Font font;
	if (!font.loadFromFile("files/Myriad_Pro.otf")) return 1;
	Text field;
	field.setFont(font);
	field.setCharacterSize(17);
	field.setFillColor(Color::Black);
	field.setStyle(Text::Style::Regular);
	field.setPosition(50, 68);
	bool read_text = false;
	String str = L"";
	Text records[128];
	Sprite states[128];
	for (int i = 0; i < 128; i++)
	{
		records[i].setString(L"");
		records[i].setFillColor(Color::Black);
		records[i].setCharacterSize(17);
		records[i].setFont(font);
		records[i].setStyle(Text::Style::Regular);
	}
	if(stage == 4)
	{
		if (!file.is_open())
			file.close();
		file.open("db.txt", std::ios_base::in);
		file.clear();
		file.seekg(0);
		int i(0);
		while (!file.eof())
		{
			file.getline(buffer, 256, '|');
			buff = buffer;
			if (buff[0] == 65279) buff.erase(0, 1);
			if (buff == L"true")
				states[i].setTexture(tick[1]);
			else if(buff == L"false")
				states[i].setTexture(tick[0]);
			file.getline(buffer, 256, '\n');
			buff = buffer;
			records[i].setString(buff);
			states[i].setPosition(68 - 20, 138 + i * 25);
			coord_sym[i] = { 68 - 20, 138 + i * 25 };
			i++;
		}
		file.close();
	}
	Event event;
	while(window.isOpen())
	{
		while(window.pollEvent(event))
		{
			window.clear(Color::White);
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;
			case Event::MouseButtonPressed:

				if (stage == 0 && pos.y >= 275 && pos.y <= 328 && pos.x >= 278 && pos.x <= 330)
					stage = 2;

				if (stage == 2)
					if (pos.y >= 60 && pos.y <= 95 && pos.x >= 48 && pos.x <= 630)
					{
						main_sprite.setTexture(main_texture[3]);
						read_text = true;
					}
					else
					{
						read_text = false;
						main_sprite.setTexture(main_texture[stage]);
					}
				if (stage == 4)
				{
					int i(0);
					const std::wstring True = L"true";
					const std::wstring False = L"false";
					std::wstring line;
					std::vector<std::wstring> lines;
					std::string::size_type position = 0;
					if (file.is_open())
						file.close();
						while (records[i].getString() != "")
						{
							if (pos.x > coord_sym[i].x && pos.x < coord_sym[i].x + 15 && pos.y > coord_sym[i].y && pos.y < coord_sym[i].y + 15)
							{
								file.open("db.txt", std::ios_base::in);
								file.clear();
								file.seekg(0);
								for(int k = 0; k < i+1; k++)
									file.getline(buffer, 256, L'\n');
								line = buffer;
							if (states[i].getTexture() == &tick[0])
							{
								while ((position = line.find(False, position)) != std::wstring::npos)
								{
									line.replace(position, False.size(), True);
									position += True.size();
								}
								states[i].setTexture(tick[1]);
							}
							else if (states[i].getTexture() == &tick[1])
							{
								while ((position = line.find(True, position)) != std::wstring::npos)
								{
									line.replace(position, True.size(), False);
									position += False.size();
								}
								states[i].setTexture(tick[0]);
							}
							lines.push_back(line);
							file.close();
							
							}
							else
							{
								file.open("db.txt", std::ios_base::in);
								file.clear();
								file.seekg(0);
								for (int k = 0; k < i + 1; k++)
									file.getline(buffer, 256, L'\n');
								line = buffer;
								file.close();
								lines.push_back(line);
							}
							i++;
						}
						file.open("db.txt", std::ios::out | std::ios::trunc);
						for (const auto& k : lines) {
							file << k;
							if (lines.back() != k)
								file << std::endl;
						}
						file.close();

					if(pos.x > 12 && pos.x < 580 && pos.y > 415 && pos.y < 469)
					{
						read_text = true;
						main_sprite.setTexture(main_texture[5]);
					}
					else if(field.getString() == "")
					{
						read_text = false;
						main_sprite.setTexture(main_texture[stage]);
					}

					if (pos.x > 590 && pos.x < 634 && pos.y > 419 && pos.y < 463)
					{
						if (file.is_open())
							file.close();
						file.open("db.txt", std::ios_base::app);
						if (!file.is_open())
							MessageBoxA(NULL, "We have problems with create a file!", "Ow!", NULL);
						//buff = str;
						file << '\n' << "false|" << buff;
						file.close();
						stage = 4;
						read_text = false;
						field.setString("");
						str.clear();
						main_sprite.setTexture(main_texture[stage]);
						if (!file.is_open())
							file.close();

						file.open("db.txt", std::ios_base::in);
						file.clear();
						file.seekg(0);
						int i(0);
						while (!file.eof())
						{
							file.getline(buffer, 256, '|');
							buff = buffer;
							if (buff[0] == 65279) buff.erase(0, 1);
							if (buff == L"true")
								states[i].setTexture(tick[1]);
							else if (buff == L"false")
								states[i].setTexture(tick[0]);
							file.getline(buffer, 256, '\n');
							buff = buffer;
							records[i].setString(buff);
							states[i].setPosition(68 - 20, 138 + i * 25);
							coord_sym[i] = { 68 - 20, 138 + i * 25 };
							if (i > 9)
								offset = i - 9;
							i++;
						}
					}
				}
				break;
			case Event::MouseMoved:
				pos = Mouse::getPosition(window);
				break;
			case Event::KeyPressed:
				if (read_text)
				{
					if (Keyboard::isKeyPressed(Keyboard::Key::BackSpace))
						if (str.getSize())
							str.erase(str.getSize() - 1, str.getSize());
					if (str.getSize() >= 50)
						break;
					if (Keyboard::isKeyPressed(Keyboard::Key::Space))
						str += ' ';
				}
				if(stage == 2)
				{
					if(Keyboard::isKeyPressed(Keyboard::Key::Return) && str.getSize())
					{
						file.open("db.txt", std::ios_base::app);
						if (!file.is_open())
							MessageBoxA(NULL, "We have problems with create a file!", "Ow!",NULL);
						buff = str;
						field.setString("");
						str.clear();
						file << "false|" << buff.data();
						file.close();
						stage = 4;
						read_text = false;
						main_sprite.setTexture(main_texture[stage]);
						if (!file.is_open())
							file.close();

						file.open("db.txt", std::ios_base::in);
						file.clear();
						file.seekg(0);
						int i(0);
						while (!file.eof())
						{
							file.getline(buffer, 256, '|');
							buff = buffer;
							if (buff[0] == 65279) buff.erase(0, 1);
							if (buff == L"true")
								states[i].setTexture(tick[1]);
							else if (buff == L"false")
								states[i].setTexture(tick[0]);
							file.getline(buffer, 256, '\n');
							buff = buffer;
							records[i].setString(buff);
							states[i].setPosition(68 - 20, 138 + i * 25);
							coord_sym[i] = { 68 - 20, 138 + i * 25 };
							i++;
						}
					}
				}
				else if (stage == 4)
				{
					field.setPosition(41, 430);
					field.setCharacterSize(20);
					if (Keyboard::isKeyPressed(Keyboard::Key::Return) && str.getSize())
					{
						if (file.is_open())
							file.close();
						file.open("db.txt", std::ios_base::app);
						if (!file.is_open())
							MessageBoxA(NULL, "We have problems with create a file!", "Ow!", NULL);
						file << '\n' << "false|" << buff;
						file.close();
						stage = 4;
						read_text = false;
						field.setString("");
						str.clear();
						main_sprite.setTexture(main_texture[stage]);
						if (!file.is_open())
							file.close();

						file.open("db.txt", std::ios_base::in);
						file.clear();
						file.seekg(0);
						int i(0);
						while (!file.eof())
						{
							file.getline(buffer, 256, '|');
							buff = buffer;
							if (buff[0] == 65279) buff.erase(0, 1);
							if (buff == L"true")
								states[i].setTexture(tick[1]);
							else if (buff == L"false")
								states[i].setTexture(tick[0]);
							file.getline(buffer, 256, '\n');
							buff = buffer;
							records[i].setString(buff);
							states[i].setPosition(68 - 20, 138 + i * 25);
							coord_sym[i] = { 68 - 20, 138 + i * 25 };
							if (i > 9)
								offset = i - 9;
							i++;
						}
						file.close();
					}
					if (Keyboard::isKeyPressed(Keyboard::Key::Down))
					{
						int i(0);
						while (records[i].getString() != "")
							i++;
						if(offset < i - 10)
							offset++;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::Up))
					{
						int i(0);
						while (records[i].getString() != "")
							i++;
						if (offset > 0)
							offset--;
					}
				}
			case Event::TextEntered:
				if (read_text)
				{
					if (str.getSize() >= 50)
						break;
					if (!Keyboard::isKeyPressed(Keyboard::Key::BackSpace))
						if ((event.text.unicode >= 65 && event.text.unicode <= 122) || (event.text.unicode >= 1040 && event.text.unicode <= 1103))
							str += static_cast<wchar_t>(event.text.unicode);
					field.setString(str);
					buff = str;
				}
				break;
			default:
				break;
			}
		}

		if (pos.y >= 275 && pos.y <= 328 && pos.x >= 278 && pos.x <= 330)
			if (stage == 0)
				main_sprite.setTexture(main_texture[1]);
			else
				main_sprite.setTexture(main_texture[stage]);

		int i(0);
		while(records[i].getString() != "")
		{
			records[i].setPosition(68, 133 + (i-offset) * 25);
			states[i].setPosition(68 - 20, 138 + (i-offset) * 25);
			i++;
		}

		window.draw(main_sprite);
		if (field.getString() != "")
			window.draw(field);
		if(stage == 4)
		{
			int i(0);
			while (records[i].getString() != "")
			{
				if(i < 10)
				{
					window.draw(records[i + offset]);
					window.draw(states[i + offset]);
				}
				i++;
			}
		}
		window.display();
	}
}
