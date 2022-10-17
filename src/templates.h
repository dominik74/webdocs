#include <string>

const std::string PAGE_TEMPLATE =
R"(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="style.css">
    <title>[tab text]</title>
</head>
<body>
    <aside>
		<h1>[nav title]</h1>
        
		[nav content]
    </aside>
    <main>
        [main content]
    </main>
</body>
</html>

)";

const std::string CSS_TEMPLATE =
R"(

body{
    display: flex;
}

aside{
    position: fixed;
    width: 300px;
    height: 100vh;
    overflow-y: auto;
}

main{
    flex: 1;
    margin-left: 310px;
}

img{
    max-width: 100%;
    max-height: 100%;
}

)";

const std::string CONFIG_TEMPLATE =
R"(tab text: "web docs"
sidebar title: "web docs"
auto-insert heading for text files: "false"
single page mode: "false"
source directory: "src"
build directory: "build")";