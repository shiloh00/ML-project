
use nalgebra::{DMat,Indexable};
use super::{BoundingBox};

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

pub fn similarity_transform() {
    unimplemented!();
}

pub fn calculate_covariance() {
    unimplemented!();
}
