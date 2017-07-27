def actual_center_and_widths(ds):
    actual_domain_widths = (ds.domain_right_edge.to_ndarray() - ds.domain_left_edge.to_ndarray()) / 1.2
    actual_le = ds.domain_left_edge.to_ndarray() + actual_domain_widths * .1
    actual_re = ds.domain_right_edge.to_ndarray() - actual_domain_widths * .1
    actual_center = (actual_le + actual_re) / 2
    return actual_domain_widths, actual_center
