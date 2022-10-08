import {h, Component} from "preact";

class RangeSingle extends Component {
    render({orientation, value, className, ...rest}, {current_value}) {
        if (isNaN(current_value))
            if (isNaN(value))
                current_value = 0;
            else
                current_value = value;
        let baseClassName = 'range input ';
        orientation = orientation || 'horizontal';
        baseClassName += orientation;
        className = className ? baseClassName + ' ' + className : baseClassName;
        return (
            <label className={className}>
                <input onInput={this.linkState('current_value')} type="range" value={current_value} {...rest}/>
                <span>{current_value}</span>
            </label>
        )
    }
}

export default RangeSingle;
