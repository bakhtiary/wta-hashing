#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>

#include <DotProductHasher.hpp>

namespace bp = boost::python;
char const* yay()
{
  return "Yay!";
}

template<typename T>
void vector_assign(std::vector<T>& l, bp::object o) {
    // Turn a Python sequence into an STL input range
    bp::stl_input_iterator<T> begin(o), end;
    l.assign(begin, end);
}

//template<typename T>
//std::vector <T> vector_vector_get(std::vector<std::vector<T>> & l) {
//    // Turn a Python sequence into an STL input range
//	std::vector<T> retval ;
//	retval.push_back(1);
//	return retval;
//}
//



BOOST_PYTHON_MODULE(libpy_dot_product_hasher)
{

  bp::def("yay", yay);

  bp::class_<WTACacheAware<float> > ("WTACacheAware",bp::init<int ,int ,int ,int , int ,int >())
		.def( "makeNewHashTable",&WTACacheAware<float>::makeNewHashTable)
		.def("lookUpHashForTheseVectors",&WTACacheAware<float>::lookUpHashForTheseVectors);

  bp::class_<WTAHasherSimple<float> > ("WTAHasherSimple",bp::init<int ,int ,int ,int , int ,int >())
		.def( "makeNewHashTable",&WTAHasherSimple<float>::makeNewHashTable)
		.def("lookUpHashForTheseVectors",&WTAHasherSimple<float>::lookUpHashForTheseVectors);

  bp::class_<LEMSimple<float> > ("LEMSimple",bp::init<int ,int , int ,int >())
    	.def( "makeNewHashTable",&LEMSimple<float>::makeNewHashTable)
    	.def("lookUpHashForTheseVectors",&LEMSimple<float>::lookUpHashForTheseVectors);

  bp::class_<ExactHighestProduct<float> > ("ExactHighestProduct",bp::init<int ,int >())
    	.def( "makeNewHashTable",&ExactHighestProduct<float>::makeNewHashTable)
    	.def("lookUpHashForTheseVectors",&ExactHighestProduct<float>::lookUpHashForTheseVectors);


  // Part of the wrapper for list<int>
  bp::class_<std::vector<float> >("vector_float")
      .def("assign", &vector_assign<float>);


  bp::class_< std::vector <int>  >("vector_int")
		.def(bp::vector_indexing_suite<std::vector <int> >())
		;

  bp::class_<std::vector <std::vector <int> > >("vector_vector_int")
         .def(bp::vector_indexing_suite<std::vector<std::vector <int> > >())
  		;



}
