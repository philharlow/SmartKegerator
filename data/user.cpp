#include <data/user.h>
#include <app.h>
#include <data/settings.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <QApplication>

using namespace std;

// Statics
int User::NextId;
User* User::UnknownUser;
vector<User*> User::UsersList;
map<int, User*> User::UsersById;

void User::LoadUsers()
{
    string filename = Settings::GetString("dataUsersFile");
    vector<StringMap> maps = DataObject::GetStringMaps(filename);

    vector<StringMap>::size_type i;
    for (i=0; i != maps.size(); i++)
    {
        User* user = new User(maps[i]);
        User::UsersById[user->Id] = user;
        User::UsersList.push_back(user);

        if (user->Id == -1)
            User::UnknownUser = user;
    }

    qDebug("%d users loaded!", User::UsersById.size());
}

void User::SaveUsers()
{
    string filename = Settings::GetString("dataUsersFile");
    string str;

    map<int, User*>::iterator iter;
    for (iter = User::UsersById.begin(); iter != User::UsersById.end(); ++iter)
    {
        str.append(iter->second->ToString());
    }

    ofstream file(filename.c_str());
    file << str;
    file.close();
}

User* User::AddUser(string name)
{
    User* user = new User();
    user->Name = name;
    user->

    User::UsersList.push_back(user);

    User::SaveUsers();

    return user;
}


// Data Object
User::User() : DataObject()
{
    Id = NextId++;
    Name = "";
    NextImageId = 0;
}

User::User(StringMap data) : DataObject(data)
{
    Id = GetInt("Id");
    NextId = max(NextId, Id+1);

    Name = GetString("Name");
    ImagePaths = GetStringVector("ImagePaths");
    NextImageId = GetInt("NextImageId");
}

string User::ToString()
{
    SetInt("Id", Id);
    SetString("Name", Name);
    SetStringVector("ImagePaths", ImagePaths);
    SetInt("NextImageId", NextImageId);

    return DataObject::ToString();
}
