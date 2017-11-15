class Test
  # Test that we resolve inside next/break (mostly a test of TreeMap,
  # which used to not recurse into these)
  def test_next_break
    each do
      if rand
        next Foo # error: unknown constant
      end
      if rand
        break Bar # error: unknown constant
      end
    end
  end
end
