

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

pub fn get_mean_shape() {
    unimplemented!();
}

pub fn project_shape() {
    unimplemented!();
}

pub fn reproject_shape() {
    unimplemented!();
}

pub fn similarity_transform() {
    unimplemented!();
}

pub fn calculate_covariance() {
    unimplemented!();
}
