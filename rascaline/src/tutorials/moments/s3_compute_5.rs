use std::sync::Arc;

use equistore::{Labels, TensorMap, LabelsBuilder, LabelValue};

use crate::{System, Error};
use crate::labels::{CenterSingleNeighborsSpeciesKeys, KeysBuilder};
use crate::labels::{AtomCenteredSamples, SamplesBuilder, SpeciesFilter};
use crate::calculators::CalculatorBase;

// these are here just to make the code below compile
const first_sample_position: Option<usize> = None;
const second_sample_position: Option<usize> = None;
const first_block_id: usize = 0;
const second_block_id: usize = 0;
const n_neighbors_first: f64 = 0.0;
const n_neighbors_second: f64 = 0.0;

#[derive(Clone, Debug)]
#[derive(serde::Serialize, serde::Deserialize)]
struct GeometricMoments {
    cutoff: f64,
    max_moment: usize,
}

impl CalculatorBase for GeometricMoments {
    fn name(&self) -> String {
        todo!()
    }

    fn parameters(&self) -> String {
        todo!()
    }

    fn keys(&self, systems: &mut [Box<dyn System>]) -> Result<Labels, Error> {
        todo!()
    }

    fn samples_names(&self) -> Vec<&str> {
        todo!()
    }

    fn samples(&self, keys: &Labels, systems: &mut [Box<dyn System>]) -> Result<Vec<Arc<Labels>>, Error> {
        todo!()
    }

    fn supports_gradient(&self, parameter: &str) -> bool {
        todo!()
    }

    fn positions_gradient_samples(&self, keys: &Labels, samples: &[Arc<Labels>], systems: &mut [Box<dyn System>]) -> Result<Vec<Arc<Labels>>, Error> {
        todo!()
    }

    fn components(&self, keys: &Labels) -> Vec<Vec<Arc<Labels>>> {
        todo!()
    }

    fn properties_names(&self) -> Vec<&str> {
        todo!()
    }

    fn properties(&self, keys: &Labels) -> Vec<Arc<Labels>> {
        todo!()
    }

    // [compute]
    fn compute(&mut self, systems: &mut [Box<dyn System>], descriptor: &mut TensorMap) -> Result<(), Error> {
        // ...

        // add this line
        let do_positions_gradients = descriptor.blocks()[0].gradient("positions").is_some();

        for (system_i, system) in systems.iter_mut().enumerate() {
            // ...
            for pair in system.pairs()? {
                // ...

                if do_positions_gradients {
                    let mut moment_gradients = Vec::new();
                    for k in 0..=self.max_moment {
                        moment_gradients.push([
                            pair.vector[0] * k as f64 * f64::powi(pair.distance, (k as i32) - 2),
                            pair.vector[1] * k as f64 * f64::powi(pair.distance, (k as i32) - 2),
                            pair.vector[2] * k as f64 * f64::powi(pair.distance, (k as i32) - 2),
                        ]);
                    }

                    if let Some(sample_position) = first_sample_position {
                        let mut block = descriptor.block_mut(first_block_id);
                        let gradient = block.gradient_mut("positions").expect("missing gradient storage");
                        let array = gradient.data.as_array_mut();

                        let gradient_wrt_second = gradient.samples.position(&[
                            sample_position.into(), system_i.into(), pair.second.into()
                        ]);
                        let gradient_wrt_self = gradient.samples.position(&[
                            sample_position.into(), system_i.into(), pair.first.into()
                        ]);

                        for (property_i, [k]) in gradient.properties.iter_fixed_size().enumerate() {
                            if let Some(sample_i) = gradient_wrt_second {
                                let grad = moment_gradients[k.usize()];
                                // There is one extra dimension in the gradients
                                // array compared to the values, accounting for
                                // each of the Cartesian directions.
                                array[[sample_i, 0, property_i]] += grad[0] / n_neighbors_first;
                                array[[sample_i, 1, property_i]] += grad[1] / n_neighbors_first;
                                array[[sample_i, 2, property_i]] += grad[2] / n_neighbors_first;
                            }

                            if let Some(sample_i) = gradient_wrt_self {
                                let grad = moment_gradients[k.usize()];
                                array[[sample_i, 0, property_i]] -= grad[0] / n_neighbors_first;
                                array[[sample_i, 1, property_i]] -= grad[1] / n_neighbors_first;
                                array[[sample_i, 2, property_i]] -= grad[2] / n_neighbors_first;
                            }
                        }
                    }

                    if let Some(sample_position) = second_sample_position {
                        let mut block = descriptor.block_mut(second_block_id);
                        let gradient = block.gradient_mut("positions").expect("missing gradient storage");
                        let array = gradient.data.as_array_mut();

                        let gradient_wrt_first = gradient.samples.position(&[
                            sample_position.into(), system_i.into(), pair.first.into()
                        ]);
                        let gradient_wrt_self = gradient.samples.position(&[
                            sample_position.into(), system_i.into(), pair.second.into()
                        ]);

                        for (property_i, [k]) in gradient.properties.iter_fixed_size().enumerate() {
                            if let Some(sample_i) = gradient_wrt_first {
                                let grad = moment_gradients[k.usize()];
                                array[[sample_i, 0, property_i]] -= grad[0] / n_neighbors_second;
                                array[[sample_i, 1, property_i]] -= grad[1] / n_neighbors_second;
                                array[[sample_i, 2, property_i]] -= grad[2] / n_neighbors_second;
                            }

                            if let Some(sample_i) = gradient_wrt_self {
                                let grad = moment_gradients[k.usize()];
                                array[[sample_i, 0, property_i]] += grad[0] / n_neighbors_second;
                                array[[sample_i, 1, property_i]] += grad[1] / n_neighbors_second;
                                array[[sample_i, 2, property_i]] += grad[2] / n_neighbors_second;
                            }
                        }
                    }
                }
            }
        }

        return Ok(());
    }
    // [compute]
}
