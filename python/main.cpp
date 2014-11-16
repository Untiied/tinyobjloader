//python3 module for tinyobjloader
//
//usage:
// import tinyobjloader as tol
// model = tol.LoadObj(name)
// print(model["shapes"])
// print(model["materials"]

#include <Python.h>
#include <vector>
#include "../tiny_obj_loader.h"

typedef std::vector<double> vectd;

PyObject*
pyTupleFromfloat3 (float array[3])
{
    int i;
    PyObject* tuple = PyTuple_New(3);

    for(i=0; i<=2 ; i++){
        PyTuple_SetItem(tuple, i, PyFloat_FromDouble(array[i]));
    }

    return tuple;
}

extern "C"
{

static PyObject*
pyLoadObj(PyObject* self, PyObject* args)
{
    PyObject *rtndict, *pyshapes, *pymaterials;
    char const* filename;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    if(!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    tinyobj::LoadObj(shapes, materials, filename);

    pyshapes = PyDict_New();
    pymaterials = PyDict_New();
    rtndict = PyDict_New();

    for (std::vector<tinyobj::shape_t>::iterator shape = shapes.begin() ;
         shape != shapes.end(); shape++)
    {
         PyObject *meshobj, *current;
         char *current_name;
         vectd vect;

         meshobj = PyDict_New();
         tinyobj::mesh_t cm  = (*shape).mesh;

         for (int i = 0; i <= 4; i++ )
         {
            current = PyList_New(0);

           if (i == 0){
                current_name = "positions";
                vect = vectd(cm.positions.begin(), cm.positions.end()); }
           else if (i==1){
                current_name = "normals";
                vect = vectd(cm.normals.begin(), cm.normals.end()); }
           else if (i == 2) {
                current_name = "texcoords";
                vect = vectd(cm.texcoords.begin(), cm.texcoords.end()); }
           else if (i==3) {
                current_name = "indicies";
                vect = vectd(cm.indices.begin(), cm.indices.end()); }
           else if (i == 4) {
                current_name = "material_ids";
                vect = vectd(cm.material_ids.begin(), cm.material_ids.end()); }

            for (vectd::iterator it = vect.begin() ;
                it != vect.end(); it++)
            {
                PyList_Insert(current, it - vect.begin(), PyFloat_FromDouble(*it));
            }

            PyDict_SetItemString(meshobj, current_name, current);
            
         }

         PyDict_SetItemString(pyshapes, (*shape).name.c_str(), meshobj);
    }

    for (std::vector<tinyobj::material_t>::iterator mat = materials.begin() ;
         mat != materials.end(); mat++)
    {
        PyObject *matobj = PyDict_New();

        PyDict_SetItemString(matobj, "shininess",  PyFloat_FromDouble((*mat).shininess));
        PyDict_SetItemString(matobj, "ior",  PyFloat_FromDouble((*mat).ior));
        PyDict_SetItemString(matobj, "dissolve",  PyFloat_FromDouble((*mat).dissolve));
        PyDict_SetItemString(matobj, "illum",  PyLong_FromLong((*mat).illum));
        PyDict_SetItemString(matobj, "ambient_texname", PyUnicode_FromString((*mat).ambient_texname.c_str()));
        PyDict_SetItemString(matobj, "diffuse_texname", PyUnicode_FromString((*mat).diffuse_texname.c_str()));
        PyDict_SetItemString(matobj, "specular_texname", PyUnicode_FromString((*mat).specular_texname.c_str()));
        PyDict_SetItemString(matobj, "normal_texname", PyUnicode_FromString((*mat).normal_texname.c_str()));
        PyDict_SetItemString(matobj, "ambient", pyTupleFromfloat3((*mat).ambient));
        PyDict_SetItemString(matobj, "diffuse", pyTupleFromfloat3((*mat).diffuse));
        PyDict_SetItemString(matobj, "specular", pyTupleFromfloat3((*mat).specular));
        PyDict_SetItemString(matobj, "transmittance", pyTupleFromfloat3((*mat).transmittance));
        PyDict_SetItemString(matobj, "emission", pyTupleFromfloat3((*mat).emission));

        PyDict_SetItemString(pymaterials, (*mat).name.c_str(), matobj);
    }

    PyDict_SetItemString(rtndict, "shapes", pyshapes);
    PyDict_SetItemString(rtndict, "materials", pymaterials);

    return rtndict;
}


static PyMethodDef mMethods[] = {

{"LoadObj", pyLoadObj, METH_VARARGS},
{NULL, NULL, 0, NULL}

};


static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "tinyobjloader",
    NULL,
    -1,
    mMethods
};


PyMODINIT_FUNC
PyInit_tinyobjloader(void)
{
    return PyModule_Create(&moduledef);
}

}
