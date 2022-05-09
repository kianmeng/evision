#ifndef EVISION_OPENCV_MAT_H
#define EVISION_OPENCV_MAT_H

#include <erl_nif.h>
#include "../nif_utils.hpp"

int get_binary_type(const std::string& t, int l, int n, int& type);

// @evision c: evision_cv_mat_empty, 0
// @evision nif: def evision_cv_mat_empty(), do: :erlang.nif_error("Mat::empty not loaded")
static ERL_NIF_TERM evision_cv_mat_empty(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    evision_res<cv::Mat *> * res;
    if (alloc_resource(&res)) {
        res->val = new cv::Mat();
    } else {
        return evision::nif::error(env, "no memory");
    }

    ERL_NIF_TERM ret = enif_make_resource(env, res);
    enif_release_resource(res);

    return evision::nif::ok(env, ret);
}

// @evision c: evision_cv_mat_type, 1
// @evision nif: def evision_cv_mat_type(_opts \\ []), do: :erlang.nif_error("Mat::type not loaded")
static ERL_NIF_TERM evision_cv_mat_type(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0))) {
            int type = img.type();
            uint8_t depth = type & CV_MAT_DEPTH_MASK;

            switch ( depth ) {
                case CV_8U:  return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "u"), enif_make_uint64(env, 8)));
                case CV_8S:  return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "s"), enif_make_uint64(env, 8)));
                case CV_16U: return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "u"), enif_make_uint64(env, 16)));
                case CV_16S: return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "s"), enif_make_uint64(env, 16)));
                case CV_32S: return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "s"), enif_make_uint64(env, 32)));
                case CV_32F: return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "f"), enif_make_uint64(env, 32)));
                case CV_64F: return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "f"), enif_make_uint64(env, 64)));
                default:     return evision::nif::ok(env, enif_make_tuple2(env, evision::nif::atom(env, "user"), enif_make_uint64(env, depth)));
            }
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_as_type, 1
// @evision nif: def evision_cv_mat_as_type(_opts \\ []), do: :erlang.nif_error("Mat::as_type not loaded")
static ERL_NIF_TERM evision_cv_mat_as_type(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;
        std::string t;
        int l = 0;
        int type;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0))) {
            if (get_binary_type(t, l, 0, type)) {
                Mat ret;
                img.convertTo(ret, type);
                return evision::nif::ok(env, evision_from(env, ret));
            } else {
                return evision::nif::error(env, "unsupported target type");
            }
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_shape, 1
// @evision nif: def evision_cv_mat_shape(_opts \\ []), do: :erlang.nif_error("Mat::shape not loaded")
static ERL_NIF_TERM evision_cv_mat_shape(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;

        // const char *keywords[] = {"img", NULL};
        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0))) {
            cv::MatSize size = img.size;
            int channels = img.channels();
            int dims = size.dims() + (channels == 1 ? 0 : 1);
            ERL_NIF_TERM* shape = (ERL_NIF_TERM *)enif_alloc(sizeof(ERL_NIF_TERM) * dims);

            for (int i = 0; i < size.dims(); i++) {
                shape[i] = enif_make_int(env, size[i]);
            }
            if (channels > 1) {
                shape[dims - 1] = enif_make_int(env, channels);
            }
            ERL_NIF_TERM ret = enif_make_tuple_from_array(env, shape, dims);
            return evision::nif::ok(env, ret);
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_clone, 1
// @evision nif: def evision_cv_mat_clone(_opts \\ []), do: :erlang.nif_error("Mat::clone not loaded")
static ERL_NIF_TERM evision_cv_mat_clone(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;

        // const char *keywords[] = {"img", NULL};
        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0))) {
            // no need to do clone here as evision_from will copy the data
            return evision::nif::ok(env, evision_from(env, img));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_to_binary, 1
// @evision nif: def evision_cv_mat_to_binary(_opts \\ []), do: :erlang.nif_error("Mat::to_binary not loaded")
static ERL_NIF_TERM evision_cv_mat_to_binary(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;
        uint64_t limit;

        // const char *keywords[] = {"img", NULL};
        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "limit"), limit, ArgInfo("limit", 0))) {
            ErlNifBinary bin_data;
            size_t bin_size;
            if (limit == 0) {
                bin_size = img.total() * img.elemSize();
            } else {
                bin_size = limit * img.elemSize();
            }

            if (!enif_alloc_binary(bin_size, &bin_data))
                return evision::nif::error(env, "alloc_failed");

            memcpy(bin_data.data, img.data, bin_size);
            return evision::nif::ok(env, enif_make_binary(env, &bin_data));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

int get_binary_type(const std::string& t, int l, int n, int& type) {
    if (t == "u") {
        if (l == 8) {
            if (n != 0) type = CV_8UC(n);
            else type = CV_8U;
            return true;
        }
        if (l == 16) {
            if (n != 0) type = CV_16UC(n);
            else type = CV_16U;
            return true;
        }
    } else if (t == "s") {
        if (l == 8) {
            if (n != 0) type = CV_8SC(n);
            else type = CV_8S;
            return true;
        }
        if (l == 16) {
            if (n != 0) type = CV_16SC(n);
            else type = CV_16S;
            return true;
        }
        if (l == 32) {
            if (n != 0) type = CV_32SC(n);
            else type = CV_32S;
            return true;
        }
    } else if (t == "f") {
        if (l == 32) {
            if (n != 0) type = CV_32FC(n);
            else type = CV_32F;
            return true;
        }
        if (l == 64) {
            if (n != 0) type = CV_64FC(n);
            else type = CV_64F;
            return true;
        }
    }
    return false;
}

// @evision c: evision_cv_mat_from_binary, 1
// @evision nif: def evision_cv_mat_from_binary(_opts \\ []), do: :erlang.nif_error("Mat::from_binary not loaded")
static ERL_NIF_TERM evision_cv_mat_from_binary(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        std::string t;
        int l = 0;
        int img_cols = 0;
        int img_rows = 0;
        int img_channels = 0;

        Mat retval;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "cols"), img_cols, ArgInfo("cols", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "rows"), img_rows, ArgInfo("rows", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "channels"), img_channels, ArgInfo("channels", 0)))
        {
            ERL_NIF_TERM erl_binary = evision_get_kw(env, erl_terms, "binary");
            ErlNifBinary data;
            if (enif_inspect_binary(env, erl_binary, &data)) {
                // validate binary data
                int type = 0;
                if (!get_binary_type(t, l, img_channels, type)) return evision::nif::error(env, "not implemented for the given type");
                size_t declared_size = img_rows * img_cols * img_channels * (l/8);
                if (declared_size != data.size) return evision::nif::error(env, "size mismatch");
                Mat tmp = Mat(img_rows, img_cols, type, (void *)data.data);
                retval = tmp.clone();
                return evision::nif::ok(env, evision_from(env, retval));
            } else {
                // invalid binary data
                return enif_make_badarg(env);
            }
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_from_binary_by_shape, 1
// @evision nif: def evision_cv_mat_from_binary_by_shape(_opts \\ []), do: :erlang.nif_error("Mat::from_binary_by_shape not loaded")
static ERL_NIF_TERM evision_cv_mat_from_binary_by_shape(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        std::string t;
        int l = 0;
        std::vector<int> shape;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "shape"), shape, ArgInfo("shape", 0))) {
            ERL_NIF_TERM erl_binary = evision_get_kw(env, erl_terms, "binary");
            ErlNifBinary data;
            if (enif_inspect_binary(env, erl_binary, &data)) {
                // validate binary data
                int type = 0;
                int ndims = (int)shape.size();
                if (!get_binary_type(t, l, 1, type)) return evision::nif::error(env, "not implemented for the given type");

                // Mat(int ndims, const int* sizes, int type, void* data, const size_t* steps=0);
                int * sizes = (int *)enif_alloc(sizeof(int) * ndims);
                for (int i = 0; i < ndims; i++) {
                    sizes[i] = shape[i];
                }
                Mat tmp = Mat(ndims, sizes, type, (void *)data.data);
                // clone here because
                //  @param data Pointer to the user data. Matrix constructors that take data and step parameters do not
                //    allocate matrix data. Instead, they just initialize the matrix header that points to the specified
                //    data, which means that no data is copied.
                Mat retval = tmp.clone();
                enif_free((void *)sizes);
                return evision::nif::ok(env, evision_from(env, retval));
            } else {
                // invalid binary data
                return enif_make_badarg(env);
            }
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_eye, 1
// @evision nif: def evision_cv_mat_eye(_opts \\ []), do: :erlang.nif_error("Mat::eye not loaded")
static ERL_NIF_TERM evision_cv_mat_eye(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        std::string t;
        int l = 0;
        uint64_t n;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "n"), n, ArgInfo("n", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0))) {
            int type = 0;
            if (!get_binary_type(t, l, 1, type)) return evision::nif::error(env, "not implemented for the given type");
            Mat out = Mat::eye(n, n, type);
            return evision::nif::ok(env, evision_from(env, out));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_reshape, 1
// @evision nif: def evision_cv_mat_reshape(_opts \\ []), do: :erlang.nif_error("Mat::reshape not loaded")
static ERL_NIF_TERM evision_cv_mat_reshape(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat mat;
        std::vector<int> shape;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "mat"), mat, ArgInfo("mat", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "shape"), shape, ArgInfo("shape", 0))) {
            Mat out = mat.reshape(0, shape);
            return evision::nif::ok(env, evision_from(env, out));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_zeros, 1
// @evision nif: def evision_cv_mat_zeros(_opts \\ []), do: :erlang.nif_error("Mat::zeros not loaded")
static ERL_NIF_TERM evision_cv_mat_zeros(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        std::vector<int> shape;
        std::string t;
        int l = 0;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "shape"), shape, ArgInfo("shape", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0))) {
            int type;
            if (!get_binary_type(t, l, 0, type)) return evision::nif::error(env, "not implemented for the given type");
            Mat out = Mat(Mat::zeros(shape.size(), shape.data(), type));
            return evision::nif::ok(env, evision_from(env, out));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_ones, 1
// @evision nif: def evision_cv_mat_ones(_opts \\ []), do: :erlang.nif_error("Mat::ones not loaded")
static ERL_NIF_TERM evision_cv_mat_ones(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        std::vector<int> shape;
        std::string t;
        int l = 0;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "shape"), shape, ArgInfo("shape", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0))) {
            int type;
            if (!get_binary_type(t, l, 0, type)) return evision::nif::error(env, "not implemented for the given type");
            Mat out = Mat(Mat::ones(shape.size(), shape.data(), type));
            return evision::nif::ok(env, evision_from(env, out));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_arange, 1
// @evision nif: def evision_cv_mat_arange(_opts \\ []), do: :erlang.nif_error("Mat::arange not loaded")
static ERL_NIF_TERM evision_cv_mat_arange(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        int64_t from, to, step;
        std::string t;
        int l = 0;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "from"), from, ArgInfo("from", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "to"), to, ArgInfo("to", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "step"), step, ArgInfo("step", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0))) {
            int type;
            if (!get_binary_type(t, l, 0, type)) return evision::nif::error(env, "not implemented for the given type");

            int64_t count = (to - from) / step;
            int dims[1] = {0};
            dims[0] = (int)count;
            if (count <= 0) return evision::nif::error(env, "invalid values for start/end/step");

            std::vector<double> values(count);
            int64_t v = from;
            for (int64_t i = 0; i < count; i++) {
                values[i] = v;
                v += step;
            }

            Mat out = Mat(1, dims, CV_64F, values.data());
            Mat ret;
            out.convertTo(ret, type);
            return evision::nif::ok(env, evision_from(env, ret));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_full, 1
// @evision nif: def evision_cv_mat_full(_opts \\ []), do: :erlang.nif_error("Mat::full not loaded")
static ERL_NIF_TERM evision_cv_mat_full(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        double number;
        std::vector<int> shape;
        std::string t;
        int l = 0;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "number"), number, ArgInfo("from", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "shape"), shape, ArgInfo("to", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "t"), t, ArgInfo("t", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "l"), l, ArgInfo("l", 0))) {
            int type;
            if (!get_binary_type(t, l, 0, type)) return evision::nif::error(env, "not implemented for the given type");

            Mat out = Mat(Mat::ones(shape.size(), shape.data(), CV_64F) * number);
            Mat ret;
            out.convertTo(ret, type);
            return evision::nif::ok(env, evision_from(env, ret));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_release, 1
// @evision nif: def evision_cv_mat_release(_opts \\ []), do: :erlang.nif_error("Mat::release not loaded")
static ERL_NIF_TERM evision_cv_mat_release(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0))) {
            img.release();
            return evision::nif::ok(env);
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_at, 1
// @evision nif: def evision_cv_mat_at(_opts \\ []), do: :erlang.nif_error("Mat::at not loaded")
static ERL_NIF_TERM evision_cv_mat_at(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;
        size_t pos;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0)) &&
            evision_to_safe(env, evision_get_kw(env, erl_terms, "pos"), pos, ArgInfo("img", 0))) {

            int type = img.type();
            uint8_t depth = type & CV_MAT_DEPTH_MASK;

            int i32;
            double f64;
            type = -1;

            switch ( depth ) {
                case CV_8U: {
                    i32 = ((uint8_t *)img.data)[pos]; type = 0;
                    break;
                }
                case CV_16U: {
                    i32 = ((uint16_t *)img.data)[pos]; type = 0;
                    break;
                }
                case CV_8S: {
                    i32 = ((int8_t *)img.data)[pos]; type = 0;
                    break;
                }
                case CV_16S: {
                    i32 = ((int16_t *)img.data)[pos]; type = 0;
                    break;
                }
                case CV_32S: {
                    i32 = ((int32_t *)img.data)[pos]; type = 0;
                    break;
                }

                case CV_32F: {
                    f64 = ((float *)img.data)[pos]; type = 1;
                    break;
                }
                case CV_64F: {
                    f64 = ((double *)img.data)[pos]; type = 1;
                    break;
                }
            }

            ERL_NIF_TERM ret;
            if (type == 0) {
                ret = evision::nif::ok(env, enif_make_int(env, i32));
            } else if (type == 1) {
                ret = evision::nif::ok(env, enif_make_double(env, f64));
            } else {
                ret = evision::nif::error(env, "unknown data type");
            }

            return ret;
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_abs, 1
// @evision nif: def evision_cv_mat_abs(_opts \\ []), do: :erlang.nif_error("Mat::abs not loaded")
static ERL_NIF_TERM evision_cv_mat_abs(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0))) {
            return evision::nif::ok(env, evision_from(env, Mat(cv::abs(img))));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

// @evision c: evision_cv_mat_expm1, 1
// @evision nif: def evision_cv_mat_expm1(_opts \\ []), do: :erlang.nif_error("Mat::expm1 not loaded")
static ERL_NIF_TERM evision_cv_mat_expm1(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace cv;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    evision::nif::parse_arg(env, nif_opts_index, argv, erl_terms);

    {
        Mat img;

        if (evision_to_safe(env, evision_get_kw(env, erl_terms, "img"), img, ArgInfo("img", 0))) {
            Mat out;
            cv::exp(img, out);
            return evision::nif::ok(env, evision_from(env, Mat(out - 1)));
        }
    }

    if (error_term != 0) return error_term;
    else return evision::nif::error(env, "overload resolution failed");
}

#endif // EVISION_OPENCV_MAT_H
