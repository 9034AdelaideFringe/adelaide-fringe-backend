#include "service/TicketService.h"
#include "utils/httplib.h"
#include <fstream>

const int SIZE = 256;

void TicketService::generateQRCode(const string &name, const string &url)
{
    httplib::Client cli("https://api.qrserver.com");

    // cli.Get("/v1/create-qr-code/?size=150x150&data=")

    char path[1024];

    snprintf(path, sizeof(path), "/v1/create-qr-code/?size=%dx%d&data=%s", SIZE, SIZE, url.c_str());

    auto r = cli.Get(path);

    if(!r)
    {
        throw runtime_error("failed when generate QRCode");
    }

    ofstream of(name);

    of << r->body;

    
}