/***************************************************************************
 *                                                                          *
 *   Copyright (C) 2020 by David B. Blumenthal and Linlin Jia               *
 *                                                                          *
 *   This file is part of GEDLIB.                                           *
 *                                                                          *
 *   GEDLIB is free software: you can redistribute it and/or modify it      *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   GEDLIB is distributed in the hope that it will be useful,              *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                          *
 ***************************************************************************/

/*!
 * @file letter_2.ipp
 * @brief ged::Letter2 class definition.
 */

#ifndef SRC_EDIT_COSTS_LETTER_2_IPP_
#define SRC_EDIT_COSTS_LETTER_2_IPP_

namespace ged {

template<>
Letter2<GXLLabel, GXLLabel>::
~Letter2() {}

template<>
Letter2<GXLLabel, GXLLabel>::
Letter2(double node_ins_cost, double node_del_cost, double node_rel_cost, double edge_ins_cost, double edge_del_cost) :
node_ins_cost_{node_ins_cost},
node_del_cost_{node_del_cost},
node_rel_cost_{node_rel_cost},
edge_ins_cost_{edge_ins_cost},
edge_del_cost_{edge_del_cost} {}

template<>
double
Letter2<GXLLabel, GXLLabel>::
node_ins_cost_fun(const GXLLabel & node_label) const {
	return node_ins_cost_;
}

template<>
double
Letter2<GXLLabel, GXLLabel>::
node_del_cost_fun(const GXLLabel & node_label) const {
	return node_del_cost_;
}

template<>
double
Letter2<GXLLabel, GXLLabel>::
node_rel_cost_fun(const GXLLabel & node_label_1, const GXLLabel & node_label_2) const {
	double x_l_minus_x_r(std::stod(node_label_1.at("x")) - std::stod(node_label_2.at("x")));
	double y_l_minus_y_r(std::stod(node_label_1.at("y")) - std::stod(node_label_2.at("y")));
	return node_rel_cost_ * std::sqrt(std::pow(x_l_minus_x_r, 2) + std::pow(y_l_minus_y_r, 2));
}

template<>
GXLLabel
Letter2<GXLLabel, GXLLabel>::
median_node_label(const std::vector<GXLLabel> & node_labels) const {
	// Transform the labels into two-dimensional coordinates and compute mean label as initial solution.
	std::vector<std::pair<double, double>> node_labels_as_coords;
	double label_x, label_y;
	double sum_x{0};
	double sum_y{0};
	for (const auto & node_label : node_labels) {
		label_x = std::stod(node_label.at("x"));
		label_y = std::stod(node_label.at("y"));
		sum_x += label_x;
		sum_y += label_y;
		node_labels_as_coords.emplace_back(label_x, label_y);
	}
	std::pair<double, double> median(sum_x / static_cast<double>(node_labels.size()), sum_y / static_cast<double>(node_labels.size()));

	// Run main loop of Weiszfeld's Algorithm.
	double epsilon{0.0001};
	double delta{1.0};
	std::size_t num_itrs{0};
	bool all_equal{false};
	while ((delta > epsilon) and (num_itrs++ < 100) and (not all_equal)) {
		std::pair<double, double> numerator(0, 0);
		double denominator{0};
		for (const auto & node_label_as_coord : node_labels_as_coords) {
			double norm{0};
			norm += (node_label_as_coord.first - median.first) * (node_label_as_coord.first - median.first);
			norm += (node_label_as_coord.second - median.second) * (node_label_as_coord.second - median.second);
			norm = std::sqrt(norm);
			if (norm > 0) {
				numerator.first += node_label_as_coord.first / norm;
				numerator.second += node_label_as_coord.second / norm;
				denominator += 1.0 / norm;
			}
		}
		if (denominator == 0) {
			all_equal = true;
		}
		else {
			std::pair<double, double> new_median(numerator.first / denominator, numerator.second / denominator);
			delta = std::abs(median.first - new_median.first) + std::abs(median.second - new_median.second);
			median = new_median;
		}
	}

	//  Transform the solution to ged::GXLLabel and return it.
	ged::GXLLabel median_label;
	median_label["x"] = std::to_string(median.first);
	median_label["y"] = std::to_string(median.second);
	return median_label;
}

template<>
double
Letter2<GXLLabel, GXLLabel>::
edge_ins_cost_fun(const GXLLabel & edge_label) const {
	return edge_ins_cost_;
}

template<>
double
Letter2<GXLLabel, GXLLabel>::
edge_del_cost_fun(const GXLLabel & edge_label) const {
	return edge_del_cost_;
}

template<>
double
Letter2<GXLLabel, GXLLabel>::
edge_rel_cost_fun(const GXLLabel & edge_label_1, const GXLLabel & edge_label_2) const {
	return 0.0;
}

}

#endif /* SRC_EDIT_COSTS_LETTER_2_IPP_ */
