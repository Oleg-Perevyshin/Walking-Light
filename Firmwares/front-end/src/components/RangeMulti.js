import {h, Component} from "preact";

class RangeMulti extends Component {
    render() {
        const { values, ids, className, ...rest } = this.props;
        const valueLow = values[0];
        const valueHigh = values[1];
        const classNames = "range multi " + className;
        return (
            <label className={classNames}>
                <input id={ids[0]} type="range" value={valueLow} {...rest} />
                <input id={ids[1]} type="range" value={valueHigh} {...rest} />
                <span>{valueLow} - {valueHigh}</span>
            </label>
        );
    }
}

export default RangeMulti;
