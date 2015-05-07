use nalgebra::{DVec,DMat,Indexable,Mean,RowSlice,Dot,Mat2,Cov};
use super::{BoundingBox};
use num::Zero;

pub struct Regressor {
    fern_list: Vec<Fern>,
}

struct Fern;

impl Regressor {
    pub fn new() -> Regressor {
        return Regressor {
            fern_list: Vec::new(),
        };
    }

    pub fn restore() {
    }

    pub fn train(&self) {
    }

    pub fn store(&self) {
    }
}

impl Fern {
    fn new() -> Fern {
        return Fern;
    }

    fn restore() {
    }

    fn train(&self) {
    }

    fn store(&self) {
    }
}

pub fn get_mean_shape(shape_list: &Vec<DMat<f64>>, bounding_box_list: &Vec<BoundingBox>) -> DMat<f64> {
    //unimplemented!();
    let nrows = shape_list[0].nrows();
    let mut mean_shape: DMat<f64> = DMat::new_zeros(nrows, 2);
    for idx in 0..shape_list.len() {
        let tmp_shape = project_shape(&shape_list[idx], &bounding_box_list[idx]);
        for ridx in 0..nrows {
            mean_shape[(ridx, 0)] += tmp_shape[(ridx, 0)];
            mean_shape[(ridx, 1)] += tmp_shape[(ridx, 1)];
        }
    }
    mean_shape / (shape_list.len() as f64)
}

pub fn project_shape(shape: &DMat<f64>, bounding_box: &BoundingBox) -> DMat<f64> {
    //unimplemented!();
    let nrows = shape.nrows();
    let mut res = DMat::<f64>::new_zeros(nrows, 2);
    for idx in 0..nrows {
        res[(idx, 0)] = (shape[(idx, 0)] - bounding_box.center_x) / (bounding_box.width / 2.0f64);
        res[(idx, 1)] = (shape[(idx, 1)] - bounding_box.center_y) / (bounding_box.height / 2.0f64);
    }
    res
}

pub fn reproject_shape(shape: &DMat<f64>, bounding_box: &BoundingBox) -> DMat<f64> {
    //unimplemented!();
    let nrows = shape.nrows();
    let mut res = DMat::<f64>::new_zeros(nrows, 2);
    for idx in 0..nrows {
        res[(idx, 0)] = shape[(idx, 0)] * bounding_box.width / 2.0f64 + bounding_box.center_x;
        res[(idx, 1)] = shape[(idx, 1)] * bounding_box.height / 2.0f64 + bounding_box.center_y;
    }
    res
}

pub fn similarity_transform(shape1: &DMat<f64>, shape2: &DMat<f64>) -> (DMat<f64>, f64) {
    //unimplemented!();
    let mut rotate_mat: DMat<f64> = DMat::new_zeros(2, 2);
    let mut scale: f64 = 0.0f64;
    let mut cx1: f64 = 0.0f64;
    let mut cy1: f64 = 0.0f64;
    let mut cx2: f64 = 0.0f64;
    let mut cy2: f64 = 0.0f64;

    assert_eq!(shape1.nrows(), shape2.nrows());
    let len: usize = shape1.nrows();

    for idx in 0..len {
        cx1 += shape1[(idx, 0)];
        cy1 += shape1[(idx, 1)];
        cx2 += shape2[(idx, 0)];
        cy2 += shape2[(idx, 1)];
    }

    cx1 /= len as f64;
    cy1 /= len as f64;
    cx2 /= len as f64;
    cy2 /= len as f64;

    let mut temp1: DMat<f64> = shape1.clone();
    let mut temp2: DMat<f64> = shape2.clone();

    for idx in 0..len {
        temp1[(idx, 0)] -= cx1;
        temp1[(idx, 1)] -= cy1;
        temp2[(idx, 0)] -= cx2;
        temp2[(idx, 1)] -= cy2;
    }

    let covar1: DMat<f64> = temp1.cov();
    let covar2: DMat<f64> = temp2.cov();
    let s1: f64 = simple_norm(&temp1);
    let s2: f64 = simple_norm(&temp2);
    scale = s1 / s2;
    temp1 = temp1 / s1;
    temp2 = temp2 / s2;
    scale = s1 / s2;

    let mut num_val: f64 = 0.0f64;
    let mut den_val: f64 = 0.0f64;

    for idx in 0..len {
        num_val += temp1[(idx, 1)] * temp2[(idx, 0)] - temp1[(idx, 0)] * temp2[(idx, 1)];
        den_val += temp1[(idx, 0)] * temp2[(idx, 0)] + temp1[(idx, 1)] * temp2[(idx, 1)];
    }

    let norm_val: f64 = (num_val * num_val + den_val * den_val).sqrt();
    let sin_theta: f64 = num_val / norm_val;
    let cos_theta: f64 = den_val / norm_val;
    rotate_mat[(0,0)] = cos_theta;
    rotate_mat[(0,1)] = -sin_theta;
    rotate_mat[(1,0)] = sin_theta;
    rotate_mat[(1,1)] = cos_theta;

    (rotate_mat, scale)
}

fn simple_norm(mat: &DMat<f64>) -> f64 {
    let mut sum: f64 = 0.0f64;
    let h: usize = mat.nrows();
    let w: usize = mat.ncols();
    for i in 0..h {
        for j in 0..w {
            let val: f64 = mat[(i, j)];
            sum += val * val;
        }
    }
    sum.sqrt()
}

pub fn calculate_covariance(vec1: &Vec<f64>, vec2: &Vec<f64>) -> f64 {
    //unimplemented!();
    let mut vm1: DMat<f64> = DMat::from_row_vec(1, vec1.len(), vec1); 
    let mut vm2: DMat<f64> = DMat::from_row_vec(1, vec2.len(), vec2); 
    let mean1 = vm1.mean()[0];
    let mean2 = vm2.mean()[0];
    assert_eq!(vm1.ncols(), vm2.ncols());
    let len: usize = vm1.ncols();
    vm1 = vm1 - mean1;
    vm2 = vm2 - mean2;
    vm1.row_slice(0, 0, len).dot(&vm2.row_slice(0, 0, len)) / (len as f64)
}
