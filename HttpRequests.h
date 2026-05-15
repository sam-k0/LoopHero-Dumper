#pragma once
#include "Helpers.h"

namespace HttpRequests
{

	YYRValue BuildGitHubHeaders()
	{
        YYRValue headers = Binds::CallBuiltinA("ds_map_create", {});

        Binds::CallBuiltinA("ds_map_add", { headers, "User-Agent",
            "Mozilla/5.0 (X11; Linux x86_64; rv:150.0) Gecko/20100101 Firefox/150.0"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Accept",
            "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Accept-Encoding",
            "gzip, deflate, br, zstd"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Accept-Language",
            "en-US,en;q=0.9"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Connection",
            "keep-alive"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Host",
            "api.github.com"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Priority",
            "u=0, i"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Sec-Fetch-Dest",
            "document"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Sec-Fetch-Mode",
            "navigate"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Sec-Fetch-Site",
            "none"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Sec-Fetch-User",
            "?1"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Sec-GPC",
            "1"
            });

        Binds::CallBuiltinA("ds_map_add", { headers, "Upgrade-Insecure-Requests",
            "1"
            });

        return headers;
	}


}