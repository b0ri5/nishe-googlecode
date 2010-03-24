#include <nishe/Graph.h>

#include <set>

using std::set;

namespace nishe {

template <typename graph_t>
bool is_automorphism(const graph_t &G, const int *x)
{
    // for each vertex
    for (int u = 0; u < G.vertex_count(); u++)
    {
        int u_x = x[u];

        // check that u's nbhd is mapped to a nbhd of equal size
        if (G.get_nbhd_size(u) != G.get_nbhd_size(u_x) )
        {
            return false;
        }

        map<vertex_t, typename graph_t::attr> nbhd_image;
        const typename graph_t::nbhr *nbhd = G.get_nbhd(u);

        // collect v_x, degree for v in u's nbhd
        for (int i = 0; i < G.get_nbhd_size(u); i++)
        {
            vertex_t v = G.nbhr_vertex(nbhd[i]);
            vertex_t v_x = x[v];

            nbhd_image[v_x] = G.nbhr_attr(nbhd[i]);
        }

        nbhd = G.get_nbhd(u_x);

        // verify that u_x's nbhd is nbhd_image
        for (int i = 0; i < G.get_nbhd_size(u_x); i++)
        {
            vertex_t v = G.nbhr_vertex(nbhd[i]);

            // if v isn't found
            if (nbhd_image.find(v) == nbhd_image.end() )
            {
                return false;
            }

            // verify that v's degree is equal to the images
            if (nbhd_image[v] != G.nbhr_attr(nbhd[i]) )
            {
                return false;
            }
        }
    }

    return true;
}

}  // namespace nishe
