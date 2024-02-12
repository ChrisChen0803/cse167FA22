/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"
#include <glm/gtx/string_cast.hpp>

// The scene init definition 
#include "RTScene.inl"


using namespace glm;
void RTScene::buildTriangleSoup(){
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader -> nlights = light.size();
    shader -> lightpositions.resize( shader -> nlights );
    shader -> lightcolors.resize( shader -> nlights );
    int count = 0;
    for (std::pair<std::string, Light*> entry : light){
        shader -> lightpositions[ count ] = (entry.second) -> position;
        shader -> lightcolors[ count ] = (entry.second) -> color;
        count++;
    }
    
    // Define stacks for depth-first search (DFS)
    std::stack < RTNode* > dfs_stack;
    std::stack < mat4 >  matrix_stack; // HW3: You will update this matrix_stack during the depth-first search while loop.
    
    // Initialize the current state variable for DFS
    RTNode* cur = node["world"]; // root of the tree
    mat4 cur_VM = glm::mat4 (1.0f); // HW3: You will update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.
    
    // HW3: The following is the beginning of the depth-first search algorithm.
    // HW3: The depth-first search for the node traversal has already been implemented (cur, dfs_stack).
    // HW3: All you have to do is to also update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
    dfs_stack.push(cur);
    /**
     * TODO: (HW3 hint: you should do something here)
     */
    matrix_stack.push(cur_VM);
    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0; 
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    
    // If you want to print some statistics of your scene graph
    // std::cout << "total numb of nodes = " << node.size() << std::endl;
    // std::cout << "total number of edges = " << total_number_of_edges << std::endl;
    
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        
        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        /**
         * TODO: (HW3 hint: you should do something here)
         */
        cur_VM = matrix_stack.top(); matrix_stack.pop();
        // draw all the models at the current node
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            // Prepare to draw the geometry. Assign the modelview and the material.
            
            /**
             * TODO: (HW3 hint: you should do something here)
             */
            auto M = cur->modeltransforms[i];
            shader -> modelview = cur_VM * M; // TODO: HW3: Without updating cur_VM, modelview would just be camera's view matrix.
            shader -> material  = ( cur -> models[i] ) -> material;
            
            // The draw command
            shader -> setUniforms();
            auto curm = cur->models[i];
            auto currT = (cur->models[i])->rtgeometry->elements;
            mat3 m3modelview; 
            m3modelview[0] = vec3(shader->modelview[0].x, shader->modelview[0].y, shader->modelview[0].z);
            m3modelview[1] = vec3(shader->modelview[1].x, shader->modelview[1].y, shader->modelview[1].z);
            m3modelview[2] = vec3(shader->modelview[2].x, shader->modelview[2].y, shader->modelview[2].z);
            mat3 viewmatrix = transpose(inverse(m3modelview));
            //std::cout << glm::to_string(shader->modelview) << std::endl;
            //std::cout << glm::to_string(m3modelview) << std::endl;
            //std::cout << glm::to_string(viewmatrix) << std::endl;
            for (int i = 0;i < currT.size();i++) {
                Triangle temp = currT[i];
                vec3 normal0 = temp.N[0];
                //std::cout << glm::to_string(normal0) << std::endl;
                normal0 = normalize(viewmatrix * normal0);
                vec3 normal1 = temp.N[1];
                //std::cout << glm::to_string(normal1) << std::endl;
                normal1 = normalize(viewmatrix * normal1);
                vec3 normal2 = temp.N[2];
                //std::cout << glm::to_string(normal2) << std::endl;
                //std::cout << "------------------" << std::endl;
                normal2 = normalize(viewmatrix * normal2);
                temp.N.push_back(normal0);
                temp.N.push_back(normal1);
                temp.N.push_back(normal2);
                
                std::cout << glm::to_string(normal0) << std::endl;
                std::cout << glm::to_string(normal1) << std::endl;
                std::cout << glm::to_string(normal2) << std::endl;
                std::cout << "///////////////////" << std::endl;
                temp.material = curm->material;
                currT[i] = temp;
            }
            triangle_soup= currT;
        }
        
        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            auto T = cur->childtransforms[i];
            /**
             * TODO: (HW3 hint: you should do something here)
             */
            matrix_stack.push(cur_VM * T);
        }
        
    } // End of DFS while loop.
    // HW3: Your code will only be above this line.
}



