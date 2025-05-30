#pragma once

#include <string>

using namespace std;


class TicketService
{
public:

    static void generateQRCode(const string& name, const string& url);
};