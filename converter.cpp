
#include "mimalloc-new-delete.h"

#include <iostream>
#include <fstream>
#include <string>

#include "clau_parser.h"

#include <map>
#include <set>


#include <filesystem>


// func - > map1, map2

// ut <-   link= { left = x  right = y }  link = { right = y  left = x }  ...

std::map<std::string, std::vector<std::string>> func(clau_parser::UserType* ut,
	std::string_view left, std::string_view right)
{
	std::map<std::string, std::vector<std::string>> result;

	for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
		std::pair<std::string, std::string> _pair;
		for (int j = 0; j < ut->GetUserTypeList(i)->GetItemListSize(); ++j) {
			auto x = ut->GetUserTypeList(i)->GetItemList(j);
			if (x.GetName() == left) {
				_pair.first = x.Get();
			}
			else if (x.GetName() == right) {
				_pair.second = x.Get();
			}
		}
		if (result.find(_pair.first) != result.end() && _pair.second.empty() == false) {
			result[_pair.first].push_back(_pair.second);
		}
		else {
			if (!_pair.second.empty()) {
				result[_pair.first] = std::vector<std::string>{ _pair.second };
			}
		}
	}

	return result;
}

std::map<std::string, std::string> func2(clau_parser::UserType* ut,
	const std::string& version,
	std::string_view left, std::string_view right)
{
	std::map<std::string, std::string> result;

	ut = ut->GetUserTypeItem(version)[0];

	for (int i = 0; i < ut->GetUserTypeListSize(); ++i) { // enter 'link = { }'
		
		std::string right_str;
		std::vector<std::string> left_vec;
		
		for (int j = 0; j < ut->GetUserTypeList(i)->GetItemListSize(); ++j) {
			auto x = ut->GetUserTypeList(i)->GetItemList(j);
			if (x.GetName() == left) {
				left_vec.push_back(x.Get());
			}
			else if (x.GetName() == right) {
				right_str = (x.Get());
			}
		}

		for (int i = 0; i < left_vec.size(); ++i) {
			result.insert(std::make_pair(left_vec[i], right_str));
		}
	}

	return result;
}

std::map<std::string, std::string> func3(clau_parser::UserType* ut,
	const std::string& version,
	std::string_view left, std::string_view right)
{
	std::map<std::string, std::string> result;

	ut = ut->GetUserTypeItem(version)[0];

	for (int i = 0; i < ut->GetUserTypeListSize(); ++i) { // enter 'link = { }'

		std::string left_str;
		std::vector<std::string> right_vec;

		for (int j = 0; j < ut->GetUserTypeList(i)->GetItemListSize(); ++j) {
			auto x = ut->GetUserTypeList(i)->GetItemList(j);
			if (x.GetName() == left) {
				left_str = x.Get();
			}
			else if (x.GetName() == right) {
				right_vec.push_back(x.Get());
			}
		}

		for (int i = 0; i < right_vec.size(); ++i) {
			result.insert(std::make_pair(left_str, right_vec[i]));
		}
	}

	return result;
}

std::map<std::string, std::vector<std::string>> chk_merge_nation(std::map<std::string, std::vector<std::string>>& _map, clau_parser::UserType* merge_nation, std::map<std::string, std::vector<std::string>>& eu3_vic2) {
	auto x = merge_nation->GetUserTypeList(0);

	for (int i = 0; i < x->GetUserTypeListSize(); ++i) {
		auto y = x->GetUserTypeList(i);

		auto master = eu3_vic2[y->GetItem("master")[0].Get()];  // eu4 tag
		auto slaves = y->GetItem("slave"); // eu3 tag

		for (auto _ : slaves) {
			if (_map.find(_.Get()) != _map.end()) {
				_map[_.Get()].clear();
				for (auto __ : master) {
					_map[_.Get()].push_back(__); // eu3 tag -> eu4 tag
					std::cout << _.Get() << " -> " << __ << "\n";
				}
			}
		}
	}


	return _map;
}

inline std::string GetEU4Country(const std::string& eu3_country, const std::map<std::string, std::vector<std::string>>& eu3_vic2, const  std::map<std::string, std::vector<std::string>>& vic2_eu4) {
	std::string x;

	try {
		auto y = eu3_vic2.at(eu3_country);
		std::vector<std::string> vec;
		for (auto _ : y) {
			auto k = vic2_eu4.at(_);

			for (auto _ : k) {
				if (!_.empty()) {
					vec.push_back(_);
				}
			}
		}

		if (!vec.empty()) {
			x = vec[0];
		}
	}
	catch (...) {
	}


	return x;
}

inline std::string GetEU3Province(const std::string& eu4_country, const std::map<std::string, std::string>& eu4_vic2, const  std::map<std::string, std::string>& vic2_eu3) {
	std::string x;

	try {
		x = vic2_eu3.at(eu4_vic2.at(eu4_country));
	}
	catch (...) {
	}

	return x; 
}

inline std::string GetEU3Province2(const std::string& eu4_country, const std::map<std::string, std::string>& eu4_vic2, const  std::map<std::string, std::string>& vic2_eu3) {
	std::string x;

	try {
		


		x = vic2_eu3.at(eu4_vic2.at(eu4_country));
	}
	catch (...) {
		//
	}

	return x;
}

inline std::string Remove(std::string x) {
	x = x.substr(1, x.size() - 2);
	return x;
}

int main(void)
{
	
	clau_parser::UserType area;

	clau_parser::LoadData::LoadDataFromFile(L"area.txt", area);

	clau_parser::UserType merge_nations_eu3_vic2;
	int count = 0;

	clau_parser::UserType eu3_vic2_country;
	clau_parser::UserType eu4_vic2_country;

	clau_parser::LoadData::LoadDataFromFile(L"merge_nations_eu3_vic2.txt", merge_nations_eu3_vic2);

	clau_parser::LoadData::LoadDataFromFile(L"country_mappings_eu3_vic2.txt", eu3_vic2_country);
	clau_parser::LoadData::LoadDataFromFile(L"country_mappings_eu4_vic2.txt", eu4_vic2_country);

	auto x = func(&eu3_vic2_country, "eu3", "vic");

	x = chk_merge_nation(x, &merge_nations_eu3_vic2, x);

	auto y = func(&eu4_vic2_country, "v2", "eu4");
	auto y2 = func(&eu4_vic2_country, "eu4", "v2");
	
	clau_parser::UserType eu3_vic2_province;
	clau_parser::UserType eu4_vic2_province;

	clau_parser::LoadData::LoadDataFromFile(L"province_mappings_eu3_vic2.txt", eu3_vic2_province);
	clau_parser::LoadData::LoadDataFromFile(L"province_mappings_eu4_vic2.txt", eu4_vic2_province);


	/// main work.
	clau_parser::UserType eu3save;

	clau_parser::LoadData::LoadDataFromFile(L"C:/Program Files (x86)/Steam/steamapps/common/Europa Universalis III - Complete/save games/Great_Britain1818_12_17.eu3", eu3save);




	std::map<std::wstring, clau_parser::UserType> eu4_provinces;

	{
		for (auto& p : std::filesystem::directory_iterator("provinces")) {

			//std::cout << p.path() << "\n";

			clau_parser::UserType eu4_province;

			clau_parser::LoadData::LoadDataFromFile(p.path(), eu4_province);
			std::wstring eu4_province_name = p.path();

			{
				auto x = eu4_province_name.rfind('/');
				if (x != std::wstring::npos) {
					eu4_province_name = eu4_province_name.substr(x + 1);
				}
				x = eu4_province_name.rfind('\\');
				if (x != std::wstring::npos) {
					eu4_province_name = eu4_province_name.substr(x + 1);
				}
			}

			{
			auto y = eu4_province_name.find(' ');
			if (y != std::string::npos) {
				eu4_province_name = eu4_province_name.substr(0, y);
			}

			auto x = eu4_province_name.find('-');
			if (x != std::string::npos) {
				eu4_province_name = eu4_province_name.substr(0, x);
			}
			}

			std::wcout << L"eu4 province name " << eu4_province_name << L"\n";
			std::vector<clau_parser::UserType*> arr;

			{
				std::string key = std::to_string(std::stoi(eu4_province_name));

				std::vector<std::string> vec;

				auto x = eu4_vic2_province.GetUserTypeItem("1.31.0.0")[0];
				bool pass = false;
				for (int i = 0; i < x->GetUserTypeListSize(); ++i) {
					auto y = x->GetUserTypeList(i);
					for (int j = 0; j < y->GetItemListSize(); ++j) {
						if (y->GetItemList(j).GetName() == "eu4" &&
							y->GetItemList(j).Get() == key) {

							for (int k = 0; k < y->GetItemListSize(); ++k) {
								if (y->GetItemList(k).GetName() == "v2") {
									vec.push_back(y->GetItemList(k).Get());
								}
							}

							pass = true;
						}
					}
				}

				if (pass) {
					auto a = eu3_vic2_province.GetUserTypeItem("dw")[0];
					bool pass = false;

					for (int q = 0; q < vec.size(); ++q) {

						const std::string key = vec[q];

						for (int i = 0; i < a->GetUserTypeListSize(); ++i) {
							auto y = a->GetUserTypeList(i);
							for (int j = 0; j < y->GetItemListSize(); ++j) {
								if (y->GetItemList(j).GetName() == "vic" &&
									y->GetItemList(j).Get() == key) {

									for (int k = 0; k < y->GetItemListSize(); ++k) {
										if (y->GetItemList(k).GetName() == "eu3") {
											auto _arr = eu3save.GetUserTypeItem(y->GetItemList(k).Get());

											for (int i = 0; i < _arr.size(); ++i) {
												arr.push_back(_arr[i]);
											}
pass = true;
										}
									}
								}
							}
						}
					}
				}
				else {

				std::cout << "eee\n";
				//	return -2;
				}
			}


			bool chk = false;


			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_province = arr[i];

				std::cout << "eu3 province " << eu3_province->GetName() << "\n";

				{
					eu4_province.RemoveUserTypeList();

					if (!eu4_province.GetItemIdx("is_city").empty()) {
						eu4_province.SetItem("is_city", "no");
					}

					// owner, controller are eu4 contry tag.
				//	std::string capital = GetEU4Country(Remove(eu3_province->GetItem("capital")[0].Get()), x, y);

					if (!eu3_province->GetItemIdx("owner").empty()) {

					//	eu4_province.AddItem("pass", "true");


						std::string owner = GetEU4Country(Remove(eu3_province->GetItem("owner")[0].Get()), x, y);
						
						if (owner.empty() == false) {
							if (eu4_province.GetItemIdx("owner").empty()) {
								eu4_province.AddItem("owner", "");
							}count++; 
							
							chk = true;
							
							eu4_province.GetItemList(eu4_province.GetItemIdx("owner")[0]).Set(0, owner);
							eu4_province.AddItem("add_core", owner);
							if (eu4_province.GetItem("is_city").empty()) {
								eu4_province.AddItem("is_city", "yes");
							}
							else {
								eu4_province.SetItem("is_city", "yes");
							}
						}
						else {
							std::cout << "chked...1\n";
						}
					}

					if (!eu3_province->GetItemIdx("controller").empty()) {
						std::string controller = GetEU4Country(Remove(eu3_province->GetItem("controller")[0].Get()), x, y);

						if (controller.empty() == false) {
							if (eu4_province.GetItemIdx("controller").empty()) {
								eu4_province.AddItem("controller", "");
							}
							eu4_province.GetItemList(eu4_province.GetItemIdx("controller")[0]).Set(0, controller);

							if (eu4_province.GetItem("is_city").empty()) {
								eu4_province.AddItem("is_city", "yes");
							}
							else {
								eu4_province.SetItem("is_city", "yes");
							}
						}
					}
					//eu4_province.GetItemList(eu4_province.GetItemIdx("capital")[0]).Set(0, capital);

					eu4_province.AddItem("discovered_by", "eastern");
					eu4_province.AddItem("discovered_by", "western");
					eu4_province.AddItem("discovered_by", "muslim");
					eu4_province.AddItem("discovered_by", "ottoman");


					eu4_province.SetName(std::to_string(std::stoi(eu4_province_name)));

					eu4_provinces[eu4_province_name] = eu4_province;
					//clau_parser::LoadData::Save(eu4_province, std::wstring(L"history/provinces/") + eu4_province_name + L".txt");
				}

				//break;
			}
			if (arr.empty()) {
				//
			}
		}
	}

	for (int i = 0; i < area.GetUserTypeListSize(); ++i) {
		auto x = area.GetUserTypeList(i);

		std::map<std::string, int> _map;

		for (int j = 0; j < x->GetItemListSize(); ++j) {
			auto y = x->GetItemList(j);
			
			auto province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))]; // eu4_province
		
			if (province.GetItem("owner").empty()) {
				continue;
			}

			if (_map.find(province.GetItem("owner")[0].Get()) != _map.end()) {
				_map[province.GetItem("owner")[0].Get()]++;
			}
			else {
				_map.insert(std::make_pair(province.GetItem("owner")[0].Get(), 0));
			}
		}

		std::string winner;
		int val = -1;

		for (auto _ : _map) {
			if (val < _.second) {
				val = _.second;
				winner = _.first;
			}
		}

		if (val != -1) { // if not 'totally empty area'.
			
			// find empty province in this area.
			for (int j = 0; j < x->GetItemListSize(); ++j) {
				auto y = x->GetItemList(j);

				auto& province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))]; // eu4_province

				if (province.GetItem("owner").empty()) {
					province.AddItem("owner", winner);
					count++;
					
					if (province.GetItem("is_city").empty()) {
						province.AddItem("is_city", "yes");
					}
					else {
						province.SetItem("is_city", "yes");
					}
				}
				if (province.GetItem("controller").empty()) {
					province.AddItem("controller", winner);
				}
			}
		}
	}

	std::cout << eu4_provinces.size() << "\n";

	for (auto x : eu4_provinces) {
		auto eu4_province = x.second;
		auto eu4_province_name = x.first;
		

		clau_parser::LoadData::Save(eu4_province, std::wstring(L"history/provinces/") + eu4_province_name + L".txt");
	}

	std::cout << "\n" << "count " << count << "\n";


	return 0;
}

