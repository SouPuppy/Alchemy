#include "alchemy/alchemy.hpp"

namespace ALCHEMY {

Object::Object(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string line;
    while (std::getline(file, line)) {
        // cout << line << endl;
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream input_line(line);
        char type;
        input_line >> type;
        switch (type) {
            case 'v': {
                float x, y, z;
                input_line >> x >> y >> z;
                vertices.push_back(Vertex(x, y, z));
                break;
            }
            case 'f': {
                int a, b, c;
                input_line >> a >> b >> c;
                faces.push_back(Face(a, b, c));
                break;
            }
            default: {
                // std::cerr << "Unknown line type: " << type << std::endl;
                break;
            }
        }
    }
    std::cout << "Successfully read in: " << path << "\n"; 
    std::cout << "VERTEX [" << vertices.size() << "]  | FACE [" << faces.size() << "]\n";
}

Triangle Object::get_triangle(int idx) {
    return Triangle(vertices[faces[idx][0] - 1], vertices[faces[idx][1] - 1], vertices[faces[idx][2] - 1]);
}

} // namespace ALCHEMY
